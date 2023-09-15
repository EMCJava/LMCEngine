//
// Created by samsa on 9/1/2023.
//

#pragma once

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <map>
#include <string>
#include <memory>

class GlobalResourcePool
{

private:
    std::map<std::string, std::shared_ptr<void>> m_Resources;

public:
    template <typename Ty>
    inline static auto STryPush( const std::string& Name, std::shared_ptr<Ty>&& Element )
    {
        return GetInstance( ).m_Resources.insert( std::make_pair( Name, std::move( Element ) ) );
    }

    template <typename Ty>
    inline static auto SSet( const std::string& Name, std::shared_ptr<Ty>&& Element )
    {
        return GetInstance( ).m_Resources[ Name ] = std::move( Element );
    }

    template <typename Ty = void>
    inline static Ty* SGet( const std::string& Name )
    {
        auto& Instance = GetInstance( );
        REQUIRED_IF( Instance.m_Resources.find( Name ) != Instance.m_Resources.end( ) )
        {
            return (Ty*) ( Instance.m_Resources[ Name ].get( ) );
        }

        return nullptr;
    }

    template <typename Ty = void>
    inline static std::shared_ptr<Ty> SGetShared( const std::string& Name )
    {
        auto& Instance = GetInstance( );
        REQUIRED_IF( Instance.m_Resources.find( Name ) != Instance.m_Resources.end( ) )
        {
            return std::static_pointer_cast<Ty>( Instance.m_Resources[ Name ] );
        }

        return nullptr;
    }

    template <typename Ty>
    inline auto TryPush( const std::string& Name, std::shared_ptr<Ty>&& Element )
    {
        return m_Resources.insert( std::make_pair( Name, std::move( Element ) ) );
    }

    template <typename Ty>
    inline auto Set( const std::string& Name, std::shared_ptr<Ty>&& Element )
    {
        return m_Resources[ Name ] = std::move( Element );
    }

    template <typename Ty = void>
    inline Ty* Get( const std::string& Name )
    {
        REQUIRED_IF( m_Resources.find( Name ) != m_Resources.end( ) )
        {
            return (Ty*) ( m_Resources.at( Name ).get( ) );
        }

        return nullptr;
    }

    template <typename Ty = void>
    inline std::shared_ptr<Ty> GetShared( const std::string& Name )
    {
        REQUIRED_IF( m_Resources.find( Name ) != m_Resources.end( ) )
        {
            return std::static_pointer_cast<Ty>( m_Resources[ Name ] );
        }

        return nullptr;
    }

    inline static void Clear( )
    {
        GetInstance( ).m_Resources.clear( );
    }

    inline void Clear( const std::string& Name )
    {
        const auto It = m_Resources.find( Name );
        if ( It != m_Resources.end( ) )
        {
            m_Resources.erase( It );
        }
    }

    static inline GlobalResourcePool&
    GetInstance( )
    {
        static GlobalResourcePool instance;
        return instance;
    }
};
