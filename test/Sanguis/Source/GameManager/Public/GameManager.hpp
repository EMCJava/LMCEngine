//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <thread>

namespace asio
{
class io_context;
}   // namespace asio
namespace SanguisNet
{
class ClientGroupParticipant;
}

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

class GameManager : public ConceptList
{
    DECLARE_CONCEPT( GameManager, ConceptList )

public:
    GameManager( );

    void
    Apply( ) override;

    void
    TestInvokable( );

private:
    std::shared_ptr<class PureConceptPerspectiveCamera> m_MainCamera;
    std::shared_ptr<class PhyControllerEntityPlayer>    m_CharController;

    /*
     *
     * Scenes
     *
     * */
    std::shared_ptr<class ConceptList> m_WandEditorScene;
    std::shared_ptr<class Reticle>     m_Reticle;

    /*
     *
     * Player status
     *
     * */
    bool       m_IsViewZooming = false;
    LinearLerp m_CameraZoomLerp;

    std::shared_ptr<asio::io_context>                   m_IOContext;
    std::shared_ptr<void>                               m_IOContextHolder;
    std::shared_ptr<SanguisNet::ClientGroupParticipant> m_ServerConnection;
    std::unique_ptr<std::thread>                        m_IOThread;

    ENABLE_IMGUI( GameManager )
};