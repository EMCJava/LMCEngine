//
// Created by samsa on 7/8/2023.
//

#include "DynamicLibrary.hpp"

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <spdlog/spdlog.h>

#include <filesystem>

#ifdef LMC_WIN
#	include <windows.h>
#else
#	error not implemented yet
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	define COPY_FILE_SUCCESS 0L
#	define SHARING_VIOLATION 32L
#elif
#	error Error code for window only
#endif

namespace
{
namespace fs = std::filesystem;

inline void
PrintSysCallError(std::string_view message)
{
	DWORD errorMessageID = ::GetLastError();

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	                             nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

	spdlog::error("{}: {}", message, std::string_view(messageBuffer, size));

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);
}

void
MakeSureOperation(auto &&Operation)
{

	while (!Operation())
		;
}

}// namespace

DynamicLibrary::DynamicLibrary(bool MakeCopyOnLoad)
{
	m_MakeCopyOnLoad = MakeCopyOnLoad;
}

DynamicLibrary::~DynamicLibrary()
{
	Unload();

	if (!m_DLLTmpPath.empty())
	{
		fs::remove(m_DLLTmpPath);
	}
}

bool
DynamicLibrary::Load(const std::string_view &Path)
{
	if (!fs::exists(Path))
	{
		spdlog::error("DynamicLibrary::Load: File {} does not exist", Path);
	}

	m_DLLLoadPath = m_DLLPath = Path;

	if (m_MakeCopyOnLoad)
	{
		MakeDLLCopy();
	}

	m_DLLHandle = LoadLibrary(m_DLLLoadPath.c_str());
	if (m_DLLHandle == nullptr)
	{
		PrintSysCallError("DynamicLibrary::Load: Can't open and load dynamic library");
		return false;
	}

	m_DLLModifiedTime = fs::last_write_time(m_DLLPath);

	LoadSymbolAs("SetEngineContext", m_SetEngineContext);
	REQUIRED(m_SetEngineContext != nullptr);

	return true;
}

void
DynamicLibrary::Unload()
{
	if (m_DLLHandle != nullptr)
	{
		if (!FreeLibrary(static_cast<HMODULE>(m_DLLHandle)))
		{
			PrintSysCallError("Failed to free dynamic library");
		}

		m_DLLHandle = nullptr;
		m_SetEngineContext = nullptr;
	}
}

bool
DynamicLibrary::Reload()
{
	Unload();
	return Load(m_DLLPath);
}

void *
DynamicLibrary::LoadSymbol(const std::string &Name)
{
	void *result = (void *)GetProcAddress(static_cast<HMODULE>(m_DLLHandle), Name.c_str());

	if (result == nullptr)
	{
		PrintSysCallError("DynamicLibrary::Load: Can't load function");
	}

	return result;
}

void
DynamicLibrary::MakeDLLCopy()
{
	RemoveDLLCopy();

	m_DLLLoadPath = m_DLLTmpPath = m_DLLPath + ".tmp";

	MakeSureOperation([this, retry_left = 10]() mutable {
		std::error_code OperationError;
		if (!fs::copy_file(m_DLLPath, m_DLLTmpPath, fs::copy_options::update_existing, OperationError))
		{
			// Success, but not overwriting
			if (COPY_FILE_SUCCESS == OperationError.value())
			{
				return true;
			}

			spdlog::error("DynamicLibrary::MakeDLLCopy: Not copying file, {}", OperationError.message());
			if (retry_left >= 1 && SHARING_VIOLATION == OperationError.value())
			{
				spdlog::error("DynamicLibrary::MakeDLLCopy: File accessing by another process, retry remaining #{}", retry_left);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				retry_left--;
				return false;
			}

			// Gave up
		}

		return true;
	});

	const std::filesystem::path DLLFilePath = m_DLLPath;
	const auto &DLLFilename = DLLFilePath.stem();
	const auto &DLLFileWithoutExtension = DLLFilePath.parent_path() / DLLFilename;

	const fs::path DLLFileDebugInfo = DLLFileWithoutExtension.string() + ".pdb";

	if (fs::exists(DLLFileDebugInfo))
	{
		spdlog::info("DynamicLibrary::MakeDLLCopy: Removing debug info for hot-reloading: {}", DLLFileDebugInfo.string());
		MakeSureOperation([&DLLFileDebugInfo, retry_left = 10]() mutable {
			std::error_code OperationError;
			if (!fs::remove(DLLFileDebugInfo, OperationError))
			{
				spdlog::error("DynamicLibrary::MakeDLLCopy: Failed to remove debug info, further hot reloading may fail, {}", OperationError.message());
				if (retry_left >= 1 && SHARING_VIOLATION == OperationError.value())
				{
					spdlog::error("DynamicLibrary::MakeDLLCopy: File accessing by another process, retry remaining #{}", retry_left);
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					retry_left--;
					return false;
				}

				// Gave up
			}

			return true;
		});
	}
}

bool
DynamicLibrary::ShouldReload() const
{
	try
	{
		const auto &NewDLLModifiedTime = fs::last_write_time(m_DLLPath);
		return NewDLLModifiedTime != m_DLLModifiedTime;
	}
	catch (const fs::filesystem_error &e)
	{
		spdlog::error("DynamicLibrary::ShouldReload: {}", e.what());
		return false;
	}
}

void
DynamicLibrary::RemoveDLLCopy()
{
	/*
	 *
	 * Existing copy, remove
	 *
	 * */
	if (!m_DLLTmpPath.empty())
	{
		fs::remove(m_DLLTmpPath);

		if (!m_DLLDependencies.empty())
		{
			std::for_each(m_DLLDependencies.begin(), m_DLLDependencies.end(), [](const auto &path) {
				fs::remove(path);
			});

			m_DLLDependencies.clear();
		}
	}
}

void
DynamicLibrary::SetEngineContext(class Engine *ptr)
{
	m_SetEngineContext(ptr);
}
