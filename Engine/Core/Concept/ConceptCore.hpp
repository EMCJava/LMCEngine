//
// Created by loyus on 7/4/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Algorithm/StringAlgorithm.hpp>
#include <Engine/Core/Algorithm/ConstexprAlg.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <set>
#include <cstdint>

#if !defined(NDEBUG) && !defined(LMC_API_EXPORTS)
#	define DEC_CHECK_ID static const IDCollisionsChecker<Concept> __IDCollisionsChecker;
#	define DEF_CHECK_ID(class_name) inline const IDCollisionsChecker<Concept> class_name::__IDCollisionsChecker{class_name::TypeID};
#else
#	define DEC_CHECK_ID
#	define DEF_CHECK_ID(class_name)
#endif

/*
 *
 * Switch case of DECLARE_CONCEPT, in-cast of standalone concept
 *
 * */
#define DECLARE_CONCEPT_BASE(class_name)                        \
public:                                                         \
	static constexpr uint64_t TypeID = HashString(#class_name); \
	using ParentSet = ConstexprContainer<TypeID>;               \
	DEC_CHECK_ID                                                \
                                                                \
public:                                                         \
	template<typename ConceptType>                              \
	static consteval bool                                       \
	CanCastS()                                                  \
	{                                                           \
		return TypeID == ConceptType::TypeID;                   \
	}                                                           \
                                                                \
	template<uint64_t ConceptID>                                \
	static consteval bool                                       \
	CanCastS()                                                  \
	{                                                           \
		return TypeID == ConceptID;                             \
	}                                                           \
                                                                \
	static constexpr bool                                       \
	CanCastSID(uint64_t ID)                                     \
	{                                                           \
		return TypeID == ID;                                    \
	}                                                           \
                                                                \
	virtual bool CanCastV(decltype(TypeID) ID);                 \
                                                                \
private:

/*
 *
 * Switch case of DECLARE_CONCEPT, in-cast of inheritance
 *
 * */
#define DECLARE_CONCEPT_INHERITED(class_name, parent_class_name)                                         \
public:                                                                                                  \
	static constexpr uint64_t TypeID = HashString(#class_name);                                          \
	using ParentSet = CombineContainers<ConstexprContainer<TypeID>, parent_class_name::ParentSet>::type; \
	DEC_CHECK_ID                                                                                         \
                                                                                                         \
public:                                                                                                  \
	template<typename ConceptType>                                                                       \
	static consteval bool                                                                                \
	CanCastS()                                                                                           \
	{                                                                                                    \
		if (TypeID == ConceptType::TypeID)                                                               \
		{                                                                                                \
			return true;                                                                                 \
		}                                                                                                \
		return parent_class_name::CanCastS<ConceptType>();                                               \
	}                                                                                                    \
                                                                                                         \
	template<uint64_t ConceptID>                                                                         \
	static consteval bool                                                                                \
	CanCastS()                                                                                           \
	{                                                                                                    \
		if (TypeID == ConceptID)                                                                         \
		{                                                                                                \
			return true;                                                                                 \
		}                                                                                                \
		return parent_class_name::CanCastS<ConceptID>();                                                 \
	}                                                                                                    \
                                                                                                         \
	static bool                                                                                          \
	CanCastSID(uint64_t ID)                                                                              \
	{                                                                                                    \
		if (TypeID == ID)                                                                                \
		{                                                                                                \
			return true;                                                                                 \
		}                                                                                                \
		return ParentSet::Contain(ID);                                                                   \
	}                                                                                                    \
                                                                                                         \
	virtual bool CanCastV(decltype(TypeID) ID) override;                                                 \
                                                                                                         \
private:

#define DECLARE_CONCEPT_SWITCH(_1, _2, NAME, ...) NAME

#define MSVC_BUG(MACRO, ARGS) MACRO ARGS// name to remind that bug fix is due to MSVC :-), I hate you very much :-))))))
#define DECLARE_CONCEPT(...) MSVC_BUG(DECLARE_CONCEPT_SWITCH(__VA_ARGS__, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_BASE), (__VA_ARGS__))

#ifdef LMC_API_EXPORTS
/*
 *
 * This is used per dynamic library to allocate a concept for that library.
 *
 * */
#	define MEM_ALLOC_CONCEPT(class_name, ...) \
		LMC_API void *mem_alloc()              \
		{                                      \
			return new class_name;             \
		}                                      \
                                               \
		LMC_API void mem_free(void *ptr)       \
		{                                      \
			delete (class_name *)ptr;          \
		}
#else
/*
 *
 * This is used per dynamic library to allocate a concept for that library.
 * Non-engine build, expand to empty
 *
 * */
#	define MEM_ALLOC_CONCEPT(class_name, ...)
#endif

/*
 *
 * Most basic concepts definition, a virtual function checking static parent set
 *
 * */
#define DEFINE_CONCEPT(class_name, ...) \
	DEF_CHECK_ID(class_name)            \
                                        \
	bool                                \
	class_name::CanCastV(uint64_t ID)   \
	{                                   \
		return ParentSet::Contain(ID);  \
	}

/*
 *
 * Most basic concepts definition + memory allocation
 * Usually used for concepts that need to be hot loaded
 * This can be used to replace DEFINE_CONCEPT_MA_SE to avoid redefinition of SetEngineContext
 *
 * */
#define DEFINE_CONCEPT_MA(class_name, ...)  \
	DEFINE_CONCEPT(class_name, __VA_ARGS__) \
	MEM_ALLOC_CONCEPT(class_name, __VA_ARGS__)

/*
 *
 * Most basic concepts definition + memory allocation + global engine context set
 * Usually used for concepts that need to be hot loaded
 *
 * */
#define DEFINE_CONCEPT_MA_SE(class_name, ...)            \
	DEFINE_CONCEPT_MA(class_name, __VA_ARGS__)           \
	LMC_API void SetEngineContext(Engine *EngineContext) \
	{                                                    \
		Engine::SetEngine(EngineContext);                \
	}

template<typename Ty>
struct IDCollisionsChecker {
	static std::set<uint64_t> id_set;
	explicit IDCollisionsChecker(uint64_t id)
	{
		REQUIRED(id_set.count(id) == 0 && "ID already in exist");
		id_set.insert(id);
	}
};

template<typename Ty>
inline std::set<uint64_t> IDCollisionsChecker<Ty>::id_set;