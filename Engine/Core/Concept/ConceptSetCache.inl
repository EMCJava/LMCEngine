//
// Created by loyus on 12/26/2023.
//

template <IsSharedPtrOrWeakPtr Container>
uint64_t
ConceptSetCache<Container>::GetHash( ) const noexcept
{
    return m_CacheHash;
}

template <IsSharedPtrOrWeakPtr Container>
void
ConceptSetCache<Container>::ForEachOriginal( auto&& F )
{
    for ( auto& C : m_CachedConcepts )
    {
        F( C );
    }
}

template <IsSharedPtrOrWeakPtr Container>
void
ConceptSetCache<Container>::ForEachRaw( auto&& F )
{
    for ( auto& C : m_CachedConcepts )
    {
        if constexpr ( std::is_same_v<Container, std::shared_ptr<typename Container::element_type>> )
        {
            F( C.get( ) );
        } else
        {
            F( C.lock( ).get( ) );
        }
    }
}

template <IsSharedPtrOrWeakPtr Container>
void
ConceptSetCache<Container>::ForEachShared( auto&& F )
{
    for ( auto& C : m_CachedConcepts )
    {
        if constexpr ( std::is_same_v<Container, std::shared_ptr<typename Container::element_type>> )
        {
            F( C );
        } else
        {
            auto Shared = C.lock( );
            F( Shared );
        }
    }
}

template <IsSharedPtrOrWeakPtr Container>
void
ConceptSetCache<Container>::ForEachOriginalIndex( auto&& F )
{
    for ( size_t i = 0; i < m_CachedConcepts.size( ); ++i )
    {
        F( i, m_CachedConcepts[ i ] );
    }
}

template <IsSharedPtrOrWeakPtr Container>
void
ConceptSetCache<Container>::ForEachRawIndex( auto&& F )
{
    for ( size_t i = 0; i < m_CachedConcepts.size( ); ++i )
    {
        if constexpr ( std::is_same_v<Container, std::shared_ptr<typename Container::element_type>> )
        {
            F( i, m_CachedConcepts[ i ].get( ) );
        } else
        {
            F( i, m_CachedConcepts[ i ].lock( ).get( ) );
        }
    }
}

template <IsSharedPtrOrWeakPtr Container>
void
ConceptSetCache<Container>::ForEachSharedIndex( auto&& F )
{
    for ( size_t i = 0; i < m_CachedConcepts.size( ); ++i )
    {
        if constexpr ( std::is_same_v<Container, std::shared_ptr<typename Container::element_type>> )
        {
            F( i, m_CachedConcepts[ i ] );
        } else
        {
            auto Shared = m_CachedConcepts[ i ].lock( );
            F( i, Shared );
        }
    }
}

template <IsSharedPtrOrWeakPtr Container>
bool
ConceptSetCache<Container>::NotEmpty( )
{
    return !m_CachedConcepts.empty( );
}

template <IsSharedPtrOrWeakPtr Container>
void
ConceptSetCache<Container>::Clear( )
{
    m_CacheHash = 0;
    m_CachedConcepts.clear( );
}
