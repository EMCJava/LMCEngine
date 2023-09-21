//
// Created by loyus on 8/21/2023.
//

#include "ToleranceBarStrike.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT_DS( ToleranceBarStrike )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ToleranceBarStrike, SpriteSquareTexture, m_LifeTimeTotal, m_LifeTimeRemain )

ToleranceBarStrike&
ToleranceBarStrike::operator<<( ToleranceBarStrike& Other )
{
    m_Coordinate        = Other.m_Coordinate;
    m_TranslationOrigin = Other.m_TranslationOrigin;
    UpdateTranslationMatrix( );

    m_Rotation       = Other.m_Rotation;
    m_RotationOrigin = Other.m_RotationOrigin;
    UpdateRotationMatrix( );

    m_Scale       = Other.m_Scale;
    m_ScaleOrigin = Other.m_ScaleOrigin;
    UpdateScaleMatrix( );

    m_LifeTimeTotal  = Other.m_LifeTimeTotal;
    m_LifeTimeRemain = Other.m_LifeTimeRemain;

    m_Shader       = Other.m_Shader;
    m_ActiveCamera = Other.m_ActiveCamera;

    m_VAO = Other.m_VAO;
    m_EBO = Other.m_EBO;
    m_VAO = Other.m_VAO;

    m_TextureID = Other.m_TextureID;

    m_ShouldDeallocateGLResources = false;

    return *this;
}

void
ToleranceBarStrike::SetLifeTime( FloatTy LiftTime )
{
    m_LifeTimeRemain = m_LifeTimeTotal = LiftTime;
}

void
ToleranceBarStrike::Render( )
{
    if ( !m_Enabled ) return;

    BindTexture( );

    if ( m_Shader != nullptr )
    {
        m_Shader->Bind( );
        if ( m_ActiveCamera != nullptr )
        {
            m_Shader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrixNonOffset( ) );
        }

        m_Shader->SetMat4( "modelMatrix", GetTranslationMatrix( ) * GetRotationMatrix( ) );

        const float Alpha = m_LifeTimeRemain / m_LifeTimeTotal;
        m_Shader->SetFloat( "alpha", Alpha );

        SpriteSquare::PureRender( );
    }

    m_LifeTimeRemain -= Engine::GetEngine( )->GetDeltaSecond( );
    if ( m_LifeTimeRemain <= 0 )
    {
        Destroy( );
    }
}
