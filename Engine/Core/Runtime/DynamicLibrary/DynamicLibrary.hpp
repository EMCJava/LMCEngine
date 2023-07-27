//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <filesystem>
#include <string>
#include <vector>

class DynamicLibrary
{
	void
	MakeDLLCopy();

	void
	RemoveDLLCopy();

public:
	DynamicLibrary(bool MakeCopyOnLoad = true);
	~DynamicLibrary();

	bool
	Load(const std::string_view &Path);

	void
	Unload();

	bool
	Reload();

	void *
	LoadSymbol(const std::string &Name);

	template<typename Ty>
	Ty
	LoadSymbolAs(const std::string &Name)
	{
		return reinterpret_cast<Ty>(LoadSymbol(Name));
	}

	template<typename Ty>
	void
	LoadSymbolAs(const std::string &Name, Ty &ptr)
	{
		ptr = reinterpret_cast<Ty>(LoadSymbol(Name));
	}

	bool
	ShouldReload() const;

	void
	SetEngineContext(class Engine *ptr);

private:
	/*
	 *
	 * The source file of the dynamic library
	 *
	 * */
	std::string m_DLLPath;

	/*
	 *
	 * The last time the dynamic library has been updated, used for reload issuing
	 *
	 * */
	std::filesystem::file_time_type m_DLLModifiedTime;

	/*
	 *
	 * The copy of the dynamic library if m_MakeCopyOnLoad set to true
	 *
	 * */
	std::string m_DLLTmpPath;

	/*
	 *
	 * Any dependencies of the dynamic library that need to be copyed, e.g. pdb file on windows
	 *
	 * */
	std::vector<std::string> m_DLLDependencies;

	/*
	 *
	 * The dynamic library actually loading, m_DLLPath or m_DLLTmpPath
	 *
	 * */
	std::string m_DLLLoadPath;

	void (*m_SetEngineContext)(class Engine *) = nullptr;

	void *m_DLLHandle = nullptr;
	bool m_MakeCopyOnLoad = true;
};
