//
// Created by loyus on 7/4/2023.
//

#pragma once

#include "ConceptCore.hpp"

#include <memory>
#include <vector>

/*
 *
 * Virtual Concept system in this engine
 *
 * */
class Concept
{
	DECLARE_CONCEPT(Concept)

	std::vector<std::unique_ptr<Concept>> m_SubConcepts;

public:
	Concept() = default;
	virtual ~Concept() = default;

	/*
	 *
	 * Operations to sub concepts
	 *
	 * */
	template<class ConceptType, typename... Args>
	ConceptType *
	AddConcept(Args &&...params);

	template<class ConceptType>
	bool
	RemoveConcept();

	template<class ConceptType>
	int
	RemoveConcepts();

	template<class ConceptType>
	ConceptType *
	GetConcept();

	template<class ConceptType>
	void
	GetConcepts(std::vector<ConceptType *> &Out);
};

template<class ConceptType, typename... Args>
ConceptType *
Concept::AddConcept(Args &&...params)
{
	return (ConceptType *)(m_SubConcepts.emplace_back(std::make_unique<ConceptType>(std::forward<Args>(params)...)).get());
}

template<class ConceptType>
ConceptType *
Concept::GetConcept()
{
	for (const auto &Concept: m_SubConcepts)
	{
		if (Concept->CanCastV(ConceptType::TypeID))
		{
			return (ConceptType *)Concept.get();
		}
	}

	return nullptr;
}

template<class ConceptType>
bool
Concept::RemoveConcept()
{
	for (size_t i = 0; i < m_SubConcepts.size(); ++i)
	{
		if (m_SubConcepts[i]->CanCastV(ConceptType::TypeID))
		{
			m_SubConcepts.erase(m_SubConcepts.begin() + i);
			return true;
		}
	}

	return false;
}

template<class ConceptType>
void
Concept::GetConcepts(std::vector<ConceptType *> &Out)
{
	Out.clear();
	for (const auto &Concept: m_SubConcepts)
	{
		if (Concept->CanCastV(ConceptType::TypeID))
		{
			Out.emplace_back((ConceptType *)Concept.get());
		}
	}
}

template<class ConceptType>
int
Concept::RemoveConcepts()
{
	return std::erase_if(m_SubConcepts.begin(), m_SubConcepts.end(),
	                     [](auto &SubConcept) {
		                     return SubConcept->template CanCast<ConceptType>();
	                     });
}