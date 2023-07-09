//
// Created by loyus on 7/9/2023.
//

#include "DynamicConcept.hpp"

#include <cassert>

DynamicConcept::~DynamicConcept()
{
	DeAllocateConcept();

	m_Concept = nullptr;
	m_DeAllocator = nullptr;
	m_Allocator = nullptr;
}

bool
DynamicConcept::Load(const std::string_view &Path, bool Allocate)
{
	bool result = DynamicLibrary::Load(Path);
	if (!result)
	{
		return false;
	}

	if (!LoadFunctions())
	{
		return false;
	}

	if (Allocate)
	{
		AllocateConcept();
	}

	return true;
}

bool
DynamicConcept::Reload(bool ReAllocate)
{
	DeAllocateConcept();

	bool result = DynamicLibrary::Reload();
	if (!result)
	{
		return false;
	}

	if (!LoadFunctions())
	{
		return false;
	}

	if (ReAllocate)
	{
		AllocateConcept();
	}

	return true;
}

bool
DynamicConcept::LoadFunctions()
{
	m_Allocator = LoadSymbolAs<void *(*)()>("mem_alloc");
	m_DeAllocator = LoadSymbolAs<void (*)(void *)>("mem_free");

	return m_Allocator != nullptr && m_DeAllocator != nullptr;
}

void
DynamicConcept::AllocateConcept()
{
	DeAllocateConcept();
	if (m_Allocator != nullptr)
	{
		m_Concept = (class Concept *)m_Allocator();
	}
}

void
DynamicConcept::DeAllocateConcept()
{
	if (m_Concept != nullptr)
	{
		assert(m_DeAllocator != nullptr);
		m_DeAllocator(m_Concept);
	}

	m_Concept = nullptr;
}
