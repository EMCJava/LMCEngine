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

	template<class ConceptType, typename... Args>
	void
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
void
Concept::AddConcept(Args &&...params)
{
	m_SubConcepts.emplace_back(std::make_unique<ConceptType>(std::forward<Args>(params)...));
}

template<class ConceptType>
ConceptType *
Concept::GetConcept()
{
	for (auto &Concept: m_SubConcepts)
	{
		if (Concept->CanCast<ConceptType>())
		{
			return (ConceptType *)this;
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
		if (m_SubConcepts[i]->CanCast<ConceptType>())
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
	for (auto &&Concept: m_SubConcepts)
	{
		if (Concept->CanCast<ConceptType>())
		{
			Out.emplace_back((ConceptType *)this);
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