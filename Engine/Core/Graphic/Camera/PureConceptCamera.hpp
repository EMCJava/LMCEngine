//
// Created by loyus on 8/1/2023.
//

#pragma once

#include "PureConceptCameraStack.hpp"

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Concept/PureConcept.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Physics/Queries/RayCast.hpp>

#include <glm/glm.hpp>

class PureConceptCamera : public PureConcept
    , public std::enable_shared_from_this<PureConceptCamera>
{
    DECLARE_CONCEPT( PureConceptCamera, PureConcept )

public:
    PureConceptCamera( );

    void
    ConceptLateInitialize( );

    [[nodiscard]] const glm::mat4&
    GetProjectionMatrix( ) const { return m_ProjectionMatrix; }
    [[nodiscard]] const glm::mat4&
    GetViewMatrix( ) const { return m_ViewMatrix; }
    [[nodiscard]] const glm::mat4&
    GetCameraMatrix( ) const { return m_CameraMatrixCache; }

    void
    SetDimensions( int Width, int Height );
    std::pair<FloatTy, FloatTy>
    GetDimensions( ) const;

    // Update projection matrices
    virtual void
    UpdateProjectionMatrix( ) = 0;

    // Update view matrices
    virtual void
    UpdateViewMatrix( ) = 0;

    // Update both projection and view matrices
    virtual void
    UpdateCameraMatrix( ) = 0;

    virtual RayCast::Ray
    ScreenSpaceToWorldSpaceRay( const glm::vec2& ScreenSpacePoint, FloatTy Distance = 0 ) const = 0;


    /*
     *
     * Camera stack controls
     *
     * */
    template <typename CameraType = PureConceptCamera>
    static CameraType* PeekCameraStack( )
    {
        const PureConceptCameraStack* CS     = Engine::GetEngine( )->GetGlobalResourcePool( )->Get<PureConceptCameraStack>( "DefaultCameraStack" );
        auto*                         ShdPtr = CS->GetCamera( ).get( );
        REQUIRED_IF( ShdPtr != nullptr && ShdPtr->CanCastVT<CameraType>( ) )
        {
            return static_cast<CameraType*>( ShdPtr );
        }

        return nullptr;
    }

    void
    PushToCameraStack( );

    void
    PopFromCameraStack( );

protected:
    glm::mat4 m_ProjectionMatrix { 1 };
    glm::mat4 m_ViewMatrix { 1 };

    // From m_ProjectionMatrix * m_ViewMatrix
    glm::mat4 m_CameraMatrixCache { 1 };

    FloatTy m_CameraWidth { }, m_CameraHeight { };

    ENABLE_IMGUI( PureConceptCamera )
};
