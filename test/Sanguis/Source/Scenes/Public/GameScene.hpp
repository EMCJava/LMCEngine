//
// Created by EMCJava on 4/3/2024.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

namespace SanguisNet
{
class ClientGroupParticipant;
class Message;
}   // namespace SanguisNet
namespace physx
{
class PxMaterial;
}   // namespace physx

struct LinearLerp {
    FloatTy Start = 0, End = 1;

    FloatTy
    Update( FloatTy DeltaTime )
    {
        if ( m_ReachEnd ) return End;

        m_InternalValue += DeltaTime;
        if ( ( m_ReachEnd = m_InternalValue >= m_OneRoundTime ) )
        {
            m_InternalValue = m_OneRoundTime;
        }

        return Get( );
    }

    [[nodiscard]] FloatTy
    Get( ) const noexcept
    {
        return ( End - Start ) * ( m_InternalValue / m_OneRoundTime ) + Start;
    }

    FloatTy
    GetInternalValue( ) const noexcept { return m_InternalValue; }

    void
    Swap( )
    {
        std::swap( Start, End );
        m_InternalValue = m_OneRoundTime - m_InternalValue;
        m_ReachEnd      = false;
    }

    void
    SetOneRoundTime( FloatTy OneRoundTime )
    {
        m_OneRoundTime = OneRoundTime;
        Reset( );
    }

    void
    Reset( ) noexcept
    {
        m_ReachEnd      = false;
        m_InternalValue = 0;
    }

    bool
    HasReachedEnd( ) const noexcept { return m_ReachEnd; }

protected:
    FloatTy m_OneRoundTime  = 1.0f;
    FloatTy m_InternalValue = 0.0f;
    bool    m_ReachEnd      = false;
};

struct PlayerStats {
    std::string Name { };
    int         Health = 100;

    [[nodiscard]] std::string ToString( ) const& noexcept
    {
        return Name + '\0' + std::to_string( Health );
    }

    static PlayerStats FromString( std::string_view Str )
    {
        return {
            .Name   = std::string( Str.data( ), Str.find( '\0' ) ),
            .Health = std::stoi( std::string { Str.begin( ) + Str.find( '\0' ) + 1, Str.end( ) } ) };
    }
};

class GameScene : public ConceptList
{
    DECLARE_CONCEPT( GameScene, ConceptList )

public:
    GameScene( std::shared_ptr<SanguisNet::ClientGroupParticipant> Connection, std::string Name );

    void
    Apply( ) override;

protected:
    void ServerMessageCallback( SanguisNet::Message& Msg );

    void DoDamage( int PlayerIndex, int Damage );

    std::shared_ptr<SanguisNet::ClientGroupParticipant> m_ServerConnection;
    std::string                                         m_UserName;

    std::shared_ptr<class PureConceptPerspectiveCamera> m_MainCamera;
    std::shared_ptr<class PhyControllerEntityPlayer>    m_CharController;

    std::vector<PlayerStats>                                    m_PlayerStats;
    std::vector<std::shared_ptr<class PostEntityUpdateWrapper>> m_PlayerControllers;

    /*
     *
     * Player status
     *
     * */
    bool                           m_IsViewZooming = false;
    LinearLerp                     m_CameraZoomLerp;
    std::shared_ptr<class Reticle> m_Reticle;

    // Physics
    physx::PxMaterial* m_DefaultPhyMaterial = nullptr;
};