//
// Created by Lo Yu Sum on 2024/01/26.
//

#include "PhyControllerEntityPlayer.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/FirstPersonCameraController.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Physics/Queries/RayCast.hpp>
#include <Engine/Core/Physics/RigidBody/RigidBody.hpp>

#include <GLFW/glfw3.h>

#include <utility>

DEFINE_CONCEPT_DS( PhyControllerEntityPlayer )

class FPSEntity : public Entity
{
    DECLARE_CONCEPT( FPSEntity, Entity )
public:
    FPSEntity( std::shared_ptr<FirstPersonCameraController> Controller )
        : m_Controller( std::move( Controller ) )
    {
        SetSearchThrough( true );
        GetOwnership( m_Controller );
    }

    void
    Apply( ) override
    {
        m_Controller->GetCamera( )->SetCameraPosition( m_Orientation.GetCoordinate( ), true );
    }

    void SetEnabled( bool Enabled ) override
    {
        m_Controller->SetEnabled( Enabled );
    }

protected:
    std::shared_ptr<FirstPersonCameraController> m_Controller;

    friend PhyControllerEntityPlayer;
};
DEFINE_CONCEPT_DS( FPSEntity )

PhyControllerEntityPlayer::PhyControllerEntityPlayer( std::shared_ptr<PureConceptPerspectiveCamera> Camera )
{
    SetSearchThrough( true );

    m_CameraController = CreateConcept<FirstPersonCameraController>( std::move( Camera ) );
    m_CameraController->SetFreeCamera( false );

    Orientation ControllerOri;
    ControllerOri.Coordinate = { 0, 1, 0 };
    AddConcept<FPSEntity>( m_CameraController ).Get( )->UpdateGlobalOrientation( ControllerOri );
}

void
PhyControllerEntityPlayer::Apply( )
{
    if ( !m_Enabled ) return;

    auto* UserInputHandle = Engine::GetEngine( )->GetUserInputHandle( );

    int FrontMovement = 0;
    int RightMovement = 0;

    if ( UserInputHandle->GetKeyState( GLFW_KEY_W ).isDown ) FrontMovement += 1;
    if ( UserInputHandle->GetKeyState( GLFW_KEY_S ).isDown ) FrontMovement -= 1;
    if ( UserInputHandle->GetKeyState( GLFW_KEY_D ).isDown ) RightMovement += 1;
    if ( UserInputHandle->GetKeyState( GLFW_KEY_A ).isDown ) RightMovement -= 1;

    glm::vec3 DeltaPosition = { };
    if ( FrontMovement != 0 ) DeltaPosition += m_CameraController->GetCamera( )->GetCameraFacing( ) * (FloatTy) FrontMovement;
    if ( RightMovement != 0 ) DeltaPosition += m_CameraController->GetCamera( )->GetCameraRightVector( ) * (FloatTy) RightMovement;
    if ( FrontMovement || RightMovement )
    {
        DeltaPosition.y = 0;
        DeltaPosition   = glm::normalize( DeltaPosition ) * 4.f;

        /*
         *
         * Speed modification
         *
         * */
        {
            if ( m_OnGround )
            {
                if ( /* Accelerated */ UserInputHandle->GetKeyState( GLFW_KEY_LEFT_CONTROL ).isDown )
                {
                    DeltaPosition *= 1.7;
                }
            } else
            {
                // Air control
                DeltaPosition *= 0.04f;
            }
        }


        constexpr auto BaseVelocity = 0.08f;
        AddFrameVelocity( DeltaPosition * BaseVelocity );
    }

    /*
     *
     * Jump
     *
     * */
    constexpr FloatTy JumpHeight = 1.25f;
    if ( m_OnGround && UserInputHandle->GetKeyState( GLFW_KEY_SPACE ).isPressed )
    {
        AddFrameVelocity( { 0, std::sqrt( 2 * JumpHeight * -m_Gravity ), 0 } );
    }

    PhyControllerEntity::Apply( );
}

void
PhyControllerEntityPlayer::SetEnabled( bool enabled )
{
    PureConcept::SetEnabled( enabled );

    GetConcept<FPSEntity>( )->SetEnabled( enabled );
}

PhyControllerEntityPlayer::operator PureConceptPerspectiveCamera*( ) const noexcept
{
    return m_CameraController->GetCamera( ).get( );
}
