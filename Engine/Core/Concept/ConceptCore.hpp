//
// Created by loyus on 7/4/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Algorithm/StringAlgorithm.hpp>

#include <set>
#include <cassert>
#include <cstdint>

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
	virtual void SetEngineContext(class Engine *EngineContext); \
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
	virtual void SetEngineContext(class Engine *EngineContext);  \
                                                                 \
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

#ifdef LMC_API_EXPORTS
#	define DEFINE_CONCEPT_MEM_ALLOC(class_name, ...) \
		LMC_API void *mem_alloc()                     \
		{                                             \
			return new class_name;                    \
		}                                             \
                                                      \
		LMC_API void mem_free(void *ptr)              \
		{                                             \
			delete (class_name *)ptr;                 \
		}
#else
#	define DEFINE_CONCEPT_MEM_ALLOC(class_name, ...)
#endif

#define DEFINE_CONCEPT_PURE(class_name, ...)                 \
	DEF_CHECK_ID(class_name)                                 \
	void class_name::SetEngineContext(Engine *EngineContext) \
	{                                                        \
		Engine::SetEngine(EngineContext);                    \
	}

#define DEFINE_CONCEPT(class_name, ...)          \
	DEFINE_CONCEPT_PURE(class_name, __VA_ARGS__) \
	DEFINE_CONCEPT_MEM_ALLOC(class_name, __VA_ARGS__)

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