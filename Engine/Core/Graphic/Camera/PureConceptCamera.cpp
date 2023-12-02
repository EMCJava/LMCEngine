//
// Created by loyus on 8/1/2023.
//

#include "PureConceptCamera.hpp"
#include "PureConceptCameraStack.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT_DS( PureConceptCamera )
DEFINE_SIMPLE_IMGUI_TYPE( PureConceptCamera, m_CameraWidth, m_CameraHeight )


PureConceptCamera::PureConceptCamera( )
{
    // Default resolution
    const auto LD  = Engine::GetEngine( )->GetLogicalMainWindowViewPortDimensions( );
    m_CameraWidth  = LD.first;
    m_CameraHeight = LD.second;
}

void
PureConceptCamera::SetDimensions( int Width, int Height )
{
    m_CameraWidth  = Width;
    m_CameraHeight = Height;

    UpdateCameraMatrix( );
}

std::pair<FloatTy, FloatTy>
PureConceptCamera::GetDimensions( ) const
{
    return { m_CameraWidth, m_CameraHeight };
}

void
PureConceptCamera::PushToCameraStack( )
{
    Engine::GetEngine( )->GetGlobalResourcePool( )->Get<PureConceptCameraStack>( "DefaultCameraStack" )->PushCamera( shared_from_this( ) );
}

void
PureConceptCamera::PopFromCameraStack( )
{
    auto ShdCamera = Engine::GetEngine( )->GetGlobalResourcePool( )->Get<PureConceptCameraStack>( "DefaultCameraStack" )->PopCamera( );
    REQUIRED( ShdCamera.get( ) == this )
}

void
PureConceptCamera::ConceptLateInitialize( )
{
    PureConcept::ConceptLateInitialize( );
    UpdateCameraMatrix( );
}
