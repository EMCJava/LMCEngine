//
// Created by loyus on 7/4/2023.
//

#pragma once

#include "ConceptCoreRuntime.hpp"

#include <Engine/Core/Algorithm/StringAlgorithm.hpp>
#include <Engine/Core/Core.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <cstdint>
#include <map>
#include <set>

template <typename Ty>
struct IDCollisionsChecker {
    static inline std::set<uint64_t> LMC_INIT_PRIORITY( 101 ) id_set { };

    // To initialize the variable
    explicit IDCollisionsChecker( ) = default;

    explicit IDCollisionsChecker( uint64_t id )
    {
        REQUIRED( !id_set.contains( id ), assert( false && "ID already in exist" ) );
        id_set.insert( id );
    }
};

inline IDCollisionsChecker<class Concept> __G_IDCollisionsChecker { };

/*
 *
 * For special ConstexprContainer
 *
 * */
#include <Engine/Core/Algorithm/ConstexprAlg.hpp>

template <typename C>
struct ConceptParentSetWrapper {
    using Container = typename C::ParentSet;
};

template <typename C>
struct ConceptValueWrapper {
    static constexpr auto Value = C::TypeID;
};

#if !defined( NDEBUG ) && defined( LMC_API_EXPORTS )
#    define DEC_CONCEPT_CHECK_ID( class_name ) inline static const IDCollisionsChecker<class Concept> __IDCollisionsChecker { class_name::TypeID };
#else
#    define DEC_CONCEPT_CHECK_ID( class_name )
#endif

#define DECLARE_CONCEPT_COMMON( class_name )                          \
public:                                                               \
    static constexpr uint64_t TypeID = HashString( #class_name );     \
    DEC_CONCEPT_CHECK_ID( class_name )                                \
    DEC_CONCEPT_NAME_COLLECTION( class_name )                         \
                                                                      \
public:                                                               \
    virtual ~class_name( );                                           \
                                                                      \
    virtual const char* GetClassName( )                               \
    {                                                                 \
        return #class_name;                                           \
    }                                                                 \
                                                                      \
    template <typename ConceptType>                                   \
    static consteval bool                                             \
    CanCastS( )                                                       \
    {                                                                 \
        return class_name::ParentSet::Contain( ConceptType::TypeID ); \
    }                                                                 \
                                                                      \
    template <uint64_t ConceptID>                                     \
    static consteval bool                                             \
    CanCastS( )                                                       \
    {                                                                 \
        return class_name::ParentSet::Contain( ConceptID );           \
    }                                                                 \
                                                                      \
    static constexpr bool                                             \
    CanCastSID( uint64_t ID )                                         \
    {                                                                 \
        return class_name::ParentSet::Contain( ID );                  \
    }                                                                 \
                                                                      \
    virtual decltype( TypeID ) GetTypeIDV( )                          \
    {                                                                 \
        return class_name::TypeID;                                    \
    }                                                                 \
                                                                      \
    virtual size_t GetSizeofV( )                                      \
    {                                                                 \
        return sizeof( class_name );                                  \
    }                                                                 \
                                                                      \
    virtual bool CanCastV( uint64_t ConceptID )                       \
    {                                                                 \
        return class_name::ParentSet::Contain( ConceptID );           \
    }                                                                 \
                                                                      \
private:
/*
 *
 * Switch case of DECLARE_CONCEPT, in-cast of standalone concept
 *
 * */
#define DECLARE_CONCEPT_BASE( class_name )              \
    DECLARE_CONCEPT_COMMON( class_name )                \
protected:                                              \
    std::string m_RuntimeName { "Anonymous Concept" };  \
                                                        \
public:                                                 \
    using ParentSet = ConstexprContainer<TypeID>;       \
                                                        \
    template <typename ConceptType>                     \
    bool                                                \
    CanCastVT( )                                        \
    {                                                   \
        return this->CanCastV( ConceptType::TypeID );   \
    }                                                   \
                                                        \
    template <class ConceptType>                        \
    bool TryCast( ConceptType*& Result )                \
    {                                                   \
        if ( CanCastV( ConceptType::TypeID ) )          \
        {                                               \
            Result = static_cast<ConceptType*>( this ); \
            return true;                                \
        }                                               \
                                                        \
        return Result = nullptr;                        \
    }                                                   \
                                                        \
    const auto&                                         \
    GetRuntimeName( )                                   \
    {                                                   \
        return m_RuntimeName;                           \
    }                                                   \
                                                        \
    void SetRuntimeName( std::string_view Name )        \
    {                                                   \
        m_RuntimeName = Name;                           \
    }                                                   \
                                                        \
private:

/*
 *
 * Switch case of DECLARE_CONCEPT, in-cast of inheritance
 *
 * */
#define DECLARE_CONCEPT_INHERITED( class_name, ... )                                                                                  \
    DECLARE_CONCEPT_COMMON( class_name )                                                                                              \
public:                                                                                                                               \
    using ParentSet = CombineContainersWrapExcludeFirst<ConceptValueWrapper, ConceptParentSetWrapper, class_name, __VA_ARGS__>::type; \
                                                                                                                                      \
private:

#define DECLARE_CONCEPT_SWITCH( _0,                                               \
                                _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,          \
                                _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                                _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                                _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                                _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                                _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
                                _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, \
                                _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, \
                                _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, \
                                _91, _92, _93, _94, _95, _96, _97, _98, _99, NAME, ... ) NAME

#define MSVC_BUG( MACRO, ARGS ) MACRO ARGS   // name to remind that bug fix is due to MSVC :-), I hate you very much :-))))))
#define DECLARE_CONCEPT( ... )  MSVC_BUG( DECLARE_CONCEPT_SWITCH( __VA_ARGS__,                                                                                                                          \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, \
                                                                 DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_INHERITED, DECLARE_CONCEPT_BASE ),    \
                                         ( __VA_ARGS__ ) )

#ifdef LMC_API_EXPORTS
/*
 *
 * This is used per dynamic library to allocate a concept for that library.
 *
 * */
#    define MEM_ALLOC_CONCEPT( class_name ) \
        LMC_API void* mem_alloc( )          \
        {                                   \
            return new class_name;          \
        }                                   \
                                            \
        LMC_API void mem_free( void* ptr )  \
        {                                   \
            delete (class_name*) ptr;       \
        }
#else
/*
 *
 * This is used per dynamic library to allocate a concept for that library.
 * Non-engine build, expand to empty
 *
 * */
#    define MEM_ALLOC_CONCEPT( class_name )
#endif

/*
 *
 * Most basic concepts definition, a virtual function checking static parent set
 *
 * */
#define DEFINE_CONCEPT( class_name )


/*
 *
 * Most basic concepts definition, a virtual function checking static parent set
 *
 * */
#define DEFINE_CONCEPT_DS( class_name )                                               \
                                                                                      \
    class_name::~class_name( )                                                        \
    {                                                                                 \
        spdlog::trace( "{}::~{} -> {}", #class_name, #class_name, fmt::ptr( this ) ); \
    }                                                                                 \
                                                                                      \
    DEFINE_CONCEPT( class_name )

/*
 *
 * Most basic concepts definition + memory allocation
 * Usually used for concepts that need to be hot loaded
 * This can be used to replace DEFINE_CONCEPT_MA_SE to avoid redefinition of SetEngineContext
 *
 * */
#define DEFINE_CONCEPT_MA( class_name ) \
    DEFINE_CONCEPT( class_name )        \
    MEM_ALLOC_CONCEPT( class_name )

/*
 *
 * Most basic concepts definition + destructor + memory allocation
 * Usually used for concepts that need to be hot loaded
 * This can be used to replace DEFINE_CONCEPT_MA_SE to avoid redefinition of SetEngineContext
 *
 * */
#define DEFINE_CONCEPT_DS_MA( class_name ) \
    DEFINE_CONCEPT_DS( class_name )        \
    MEM_ALLOC_CONCEPT( class_name )

/*
 *
 * Most basic concepts definition + memory allocation + global engine context set
 * Usually used for concepts that need to be hot loaded
 *
 * */
#define DEFINE_CONCEPT_MA_SE( class_name )                 \
    DEFINE_CONCEPT_MA( class_name )                        \
    LMC_API void SetEngineContext( Engine* EngineContext ) \
    {                                                      \
        Engine::SetEngine( EngineContext );                \
    }

/*
 *
 * Most basic concepts definition + destructor + memory allocation + global engine context set
 * Usually used for concepts that need to be hot loaded
 *
 * */
#define DEFINE_CONCEPT_DS_MA_SE( class_name )              \
    DEFINE_CONCEPT_DS( class_name )                        \
    MEM_ALLOC_CONCEPT( class_name )                        \
    LMC_API void SetEngineContext( Engine* EngineContext ) \
    {                                                      \
        Engine::SetEngine( EngineContext );                \
    }