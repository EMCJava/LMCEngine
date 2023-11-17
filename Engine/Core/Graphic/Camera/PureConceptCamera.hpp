//
// Created by loyus on 8/1/2023.
//

#pragma once

#include "PureConceptCameraStack.hpp"

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Concept/PureConcept.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>

#include <glm/glm.hpp>

class PureConceptCamera : public PureConcept
    , public std::enable_shared_from_this<PureConceptCamera>
{
    DECLARE_CONCEPT( PureConceptCamera, PureConcept )

public:
    PureConceptCamera( );

    void
    ConceptLaterInitialize( );

    [[nodiscard]] const glm::mat4&
    GetProjectionMatrix( ) const;

    void
    SetDimensions( int Width, int Height );
    std::pair<FloatTy, FloatTy>
    GetDimensions( ) const;

    virtual void
    UpdateProjectionMatrix( ) = 0;

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
    FloatTy   m_CameraWidth { }, m_CameraHeight { };

    ENABLE_IMGUI( PureConceptCamera )
};
