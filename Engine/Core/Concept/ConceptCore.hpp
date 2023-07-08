//
// Created by loyus on 7/4/2023.
//

#pragma once

#include <Engine/Core/Algorithm/StringAlgorithm.hpp>

#include <set>
#include <assert.h>
#include <stdint.h>

#if !defined(NDEBUG) && !defined(LMC_API_EXPORTS)
#	define DEC_CHECK_ID static const IDCollisionsChecker<Concept> __IDCollisionsChecker;
#	define DEF_CHECK_ID(class_name) inline const IDCollisionsChecker<Concept> class_name::__IDCollisionsChecker{class_name::TypeID};
#else
#	define DEC_CHECK_ID
#	define DEF_CHECK_ID(class_name)
#endif

#define DECLARE_CONCEPT_BASE(class_name)                        \
public:                                                         \
	static constexpr uint64_t TypeID = HashString(#class_name); \
	DEC_CHECK_ID                                                \
public:                                                         \
	template<typename ConceptType>                              \
	static consteval bool                                       \
	CanCast()                                                   \
	{                                                           \
		return TypeID == ConceptType::TypeID;                   \
	}                                                           \
                                                                \
private:

#define DECLARE_CONCEPT_INHERITED(class_name, parent_class_name) \
public:                                                          \
	static constexpr uint64_t TypeID = HashString(#class_name);  \
	DEC_CHECK_ID                                                 \
public:                                                          \
	template<typename ConceptType>                               \
	static consteval bool                                        \
	CanCast()                                                    \
	{                                                            \
		if (TypeID == ConceptType::TypeID)                       \
		{                                                        \
			return true;                                         \
		}                                                        \
		return parent_class_name::CanCast<ConceptType>();        \
	}                                                            \
                                                                 \
private:

#define DECLARE_CONCEPT_SWITCH(_1, _2, NAME, ...) NAME
#define DECLARE_CONCEPT(...) DECLARE_CONCEPT_SWITCH(__VA_ARGS__, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_BASE)(__VA_ARGS__)

#define DEFINE_CONCEPT(class_name, ...) \
	DEF_CHECK_ID(class_name)

template<typename Ty>
struct IDCollisionsChecker {
	static std::set<uint64_t> id_set;
	explicit IDCollisionsChecker(uint64_t id)
	{
		assert(id_set.count(id) == 0 && "ID already in exist");
		id_set.insert(id);
	}
};

template<typename Ty>
inline std::set<uint64_t> IDCollisionsChecker<Ty>::id_set;