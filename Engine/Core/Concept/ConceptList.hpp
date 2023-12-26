//
// Created by samsa on 12/14/2023.
//

#pragma once

#include "PureConcept.hpp"

#include <Engine/Core/Concept/ConceptSetCache.hpp>

class ConceptList : public PureConcept
{
    DECLARE_CONCEPT( ConceptList, PureConcept )

public:
    // Avoid unnecessary casting/copying if return value is not used
    template <class UnderlyingType, class Ty>
    struct SharedPtrProxy {
        SharedPtrProxy( std::shared_ptr<UnderlyingType>& SharedObjectRef )
            : SharedObjectRef( SharedObjectRef )
        { }

        operator std::shared_ptr<Ty>( )
        {
            return Get( );
        }

        operator std::weak_ptr<Ty>( )
        {
            return Get( );
        }

        auto Get( )
        {
            return ConceptCasting<Ty>( SharedObjectRef );
        }

        std::shared_ptr<UnderlyingType>& SharedObjectRef;
    };

    // Avoid returning a copy of shared_ptr for nullptr pointer compatible
    template <class Ty>
    using OptionalRef = std::optional<std::reference_wrapper<std::decay<Ty>>>;

    /*
     *
     * Construct and append concept
     *
     * */
    template <class ConceptType, typename... Args>
    SharedPtrProxy<PureConcept, ConceptType>
    AddConcept( Args&&... params );

    /*
     *
     * Remove a concept by pointer
     * return true if success
     *
     * */
    bool
    RemoveConcept( PureConcept* ConceptPtr );

    /*
     *
     * Remove single concept by type
     * return true if success
     *
     * */
    template <class ConceptType>
    bool
    RemoveConcept( );

    /*
     *
     * Remove all concepts of type
     * return number of removed concepts
     *
     * */
    template <class ConceptType>
    int
    RemoveConcepts( );

    /*
     *
     * Get a concept by index in range [0, size)
     *
     * */
    template <class ConceptType = PureConcept>
    [[nodiscard]] std::shared_ptr<ConceptType>
    GetConceptAt( size_t SubConceptIndex ) const noexcept;

    /*
     *
     *
     *
     * */
    [[nodiscard]] size_t
    GetSubConceptCount( ) const noexcept { return m_SubConcepts.size( ); }

    /*
     *
     * Returns the first matching concept
     *
     * */
    template <class ConceptType = PureConcept>
    [[nodiscard]] std::shared_ptr<ConceptType>
    GetConcept( ) const noexcept;

    /*
     *
     * If the template argument is not specified,
     * Element type of the Container will be fetched
     *
     * */
    template <class ConceptType = void, IsSharedPtrOrWeakPtr Container = std::shared_ptr<PureConcept>>
    void
    GetConcepts( std::vector<Container>& Out, bool CanSearchThrough = true ) const;

    /*
     *
     * Won't update the cache if CacheHash is same as last time (no change to sub-concept)
     *
     * If the template argument is not specified,
     * Element type of the Container will be fetched
     *
     * */
    template <class ConceptType = void, IsSharedPtrOrWeakPtr Container = std::shared_ptr<PureConcept>>
    void
    GetConcepts( ConceptSetCache<Container>& Out, bool CanSearchThrough = true ) const;

    /*
     *
     * Return the first name matching concept
     *
     * */
    template <class ConceptType = PureConcept>
    std::shared_ptr<ConceptType>
    GetConceptByName( std::string_view Name, bool CanSearchThrough = true ) const;

    /*
     *
     * A simple for-each loop on the list
     * lambda/function passed in should a shared_ptr reference argument
     * Template argument specifies the type of the shared_ptr, default is PureConcept
     *
     * */
    template <class ConceptType = PureConcept>
    void
    ForEachSubConcept( auto&& Func );

    /*
     *
     * Returns true if the list is not empty
     *
     * */
    [[nodiscard]] bool
    HasSubConcept( ) const noexcept { return !m_SubConcepts.empty( ); }

    /*
     *
     * Get ownership of a sub concept
     * Action: Remove from old owner(needed) and add to new owner(this)
     * Return false if no action is performed
     *
     * */
    bool
    GetOwnership( PureConcept* ConceptPtr );

    /*
     *
     * Get ownership of a sub concept, it also works when concept is not previously owned by other
     * Action: Remove from old owner(only if not shared and owned, not required) and add to new owner(this)
     * Return false if no action is performed
     *
     * */
    template <typename ConceptType>
    bool
    GetOwnership( std::shared_ptr<ConceptType> ConceptPtr );

    /*
     *
     * Set when doing a search, whether recursive search on sub-ConceptList is performed
     *
     * */
    void
    SetSearchThrough( bool SearchThrough = true );

    /*
     *
     * Reset hash, and owner's hash if m_SearchThrough == true
     *
     * */
    void
    ResetSubConceptCache( );

private:
    /*
     *
     * Same as GetConcepts but will not clear the vector
     *
     * */
    template <class ConceptType, IsSharedPtrOrWeakPtr Container>
    void
    GetConcepts_Internal( std::vector<Container>& Out, bool CanSearchThrough = true ) const;

private:
    std::vector<std::shared_ptr<PureConcept>> m_SubConcepts;

    /*
     *
     * If this is set to true, when searching for "a SET of" concepts, it will also check for its sub-concept
     *
     * */
    bool m_SearchThrough = false;
    friend class PureConcept;
};

template <class ConceptType, typename... Args>
auto
ConceptList::AddConcept( Args&&... params ) -> SharedPtrProxy<PureConcept, ConceptType>
{
    ResetSubConceptCache( );
    static_assert( ConceptType::template CanCastS<PureConcept>( ) );

    auto Result         = CreateConcept<ConceptType>( std::forward<Args>( params )... );
    Result->m_BelongsTo = this;

    return m_SubConcepts.emplace_back( std::move( Result ) );
}

template <class ConceptType>
std::shared_ptr<ConceptType>
ConceptList::GetConcept( ) const noexcept
{
    for ( auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            return ConceptCasting<ConceptType>( Concept );
        }
    }

    return nullptr;
}

template <class ConceptType>
std::shared_ptr<ConceptType>
ConceptList::GetConceptAt( size_t SubConceptIndex ) const noexcept
{
    REQUIRED_IF( SubConceptIndex < m_SubConcepts.size( ) )
    {
        return ConceptCasting<ConceptType>( m_SubConcepts[ SubConceptIndex ] );
    }

    return nullptr;
}

template <class ConceptType>
bool
ConceptList::RemoveConcept( )
{
    for ( size_t i = 0; i < m_SubConcepts.size( ); ++i )
    {
        if ( m_SubConcepts[ i ]->CanCastV( ConceptType::TypeID ) )
        {
            TEST( m_SubConcepts[ i ]->m_BelongsTo == this )
            m_SubConcepts[ i ]->m_BelongsTo = nullptr;

            m_SubConcepts.erase( m_SubConcepts.begin( ) + i );
            ResetSubConceptCache( );
            return true;
        }
    }

    return false;
}

template <class ConceptType, IsSharedPtrOrWeakPtr Container>
void
ConceptList::GetConcepts_Internal( std::vector<Container>& Out, bool CanSearchThrough ) const
{
    const auto CheckSubConcept = [ &Out ]( const std::shared_ptr<PureConcept>& ConceptSharePtr ) {
        const auto* const ConceptPtr = (ConceptList*) ConceptSharePtr.get( );

        // Need to check for sub-concept
        if ( ConceptPtr->m_SearchThrough )
        {
            ConceptPtr->GetConcepts_Internal<ConceptType>( Out );
        }
    };

    for ( const auto& ConceptSharePtr : m_SubConcepts )
    {
        if ( ConceptSharePtr->CanCastV( ConceptType::TypeID ) )
        {
            if constexpr ( std::is_same_v<PureConcept, typename Container::element_type> )
            {
                Out.emplace_back( ConceptSharePtr );
            } else
            {
                Out.emplace_back( ConceptCasting<typename Container::element_type>( ConceptSharePtr ) );
            }

            // Can avoid calling virtual functions
            if constexpr ( ConceptType::template CanCastS<ConceptList>( ) )
            {
                CheckSubConcept( ConceptSharePtr );
                continue;
            }
        }

        // No sub-concept
        if ( !CanSearchThrough || !ConceptSharePtr->CanCastV( ConceptList::TypeID ) )
        {
            continue;
        }

        CheckSubConcept( ConceptSharePtr );
    }
}

template <class ConceptType, IsSharedPtrOrWeakPtr Container>
void
ConceptList::GetConcepts( std::vector<Container>& Out, bool CanSearchThrough ) const
{
    Out.clear( );

    // Not specified
    if constexpr ( std::is_same_v<ConceptType, void> )
    {
        GetConcepts_Internal<typename Container::element_type>( Out, CanSearchThrough );
    } else
    {
        GetConcepts_Internal<ConceptType>( Out, CanSearchThrough );
    }
}

template <typename ConceptType>
bool
ConceptList::GetOwnership( std::shared_ptr<ConceptType> ConceptPtr )
{
    if ( ConceptPtr == nullptr ) [[unlikely]]
    {
        return false;
    }

    /*
     *
     * Dangling concept
     *
     * */
    if ( ConceptPtr->m_BelongsTo == nullptr )
    {
        ConceptPtr->m_BelongsTo = this;
        m_SubConcepts.emplace_back( std::move( ConceptPtr ) );
        ResetSubConceptCache( );
        return true;
    }

    return GetOwnership( ConceptPtr.get( ) );
}

template <class ConceptType>
int
ConceptList::RemoveConcepts( )
{
    auto PartIt = std::partition( m_SubConcepts.begin( ), m_SubConcepts.end( ),
                                  []( auto& SubConcept ) {
                                      return SubConcept->template CanCastVT<ConceptType>( );
                                  } );

    const auto RemoveCount = std::distance( m_SubConcepts.begin( ), PartIt );
    if ( RemoveCount != 0 ) [[likely]]
    {
        std::for_each( m_SubConcepts.begin( ), PartIt, [ this ]( auto& SubConcept ) {
            TEST( SubConcept->m_BelongsTo == this );
            SubConcept->m_BelongsTo = nullptr;
        } );

        m_SubConcepts.erase( m_SubConcepts.begin( ), PartIt );
        ResetSubConceptCache( );
    }

    return RemoveCount;
}

template <class ConceptType, IsSharedPtrOrWeakPtr Container>
void
ConceptList::GetConcepts( ConceptSetCache<Container>& Out, bool CanSearchThrough ) const
{
    const auto StateHash = GetHash( );
    if ( Out.m_CacheHash == StateHash )
    {
        // Concept set not changed
        return;
    }

    Out.m_CacheHash = StateHash;
    GetConcepts<ConceptType>( Out.m_CachedConcepts, CanSearchThrough );
}

template <class ConceptType>
std::shared_ptr<ConceptType>
ConceptList::GetConceptByName( std::string_view Name, bool CanSearchThrough ) const
{
    for ( auto& ConceptSharePtr : m_SubConcepts )
    {
        if ( ConceptSharePtr->GetRuntimeName( ) == Name )
        {
            return ConceptSharePtr;
        }

        // No sub-concept / can't search through
        if ( !CanSearchThrough || !( ConceptSharePtr->CanCastV( ConceptList::TypeID ) && ( (ConceptList*) ConceptSharePtr.get( ) )->m_SearchThrough ) )
        {
            continue;
        }

        if ( auto Result = ( (ConceptList*) ConceptSharePtr.get( ) )->GetConceptByName( Name, CanSearchThrough ) )
        {
            return Result;
        }
    }
}

template <class ConceptType>
void
ConceptList::ForEachSubConcept( auto&& Func )
{
    for ( auto& ConceptSharePtr : m_SubConcepts )
    {
        if ( std::is_same_v<PureConcept, ConceptType> || ConceptSharePtr->CanCastV( ConceptType::TypeID ) )
        {
            if constexpr ( std::is_same_v<PureConcept, ConceptType> )
            {
                Func( ConceptSharePtr );
            } else
            {
                Func( std::dynamic_pointer_cast<ConceptType>( ConceptSharePtr ) );
            }
        }
    }
}