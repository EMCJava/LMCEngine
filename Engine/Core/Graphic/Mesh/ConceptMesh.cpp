//
// Created by samsa on 10/23/2023.
//

#include "ConceptMesh.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( ConceptMesh )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ConceptMesh, ConceptRenderable, m_VAO, m_VBO, m_EBO )

void
ConceptMesh::Render( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    m_Shader->Bind( );
    REQUIRED_IF( m_ActiveCamera != nullptr && m_ActiveCamera->CanCastVT<PureConceptPerspectiveCamera>( ) )
    {
        auto* PC = (PureConceptPerspectiveCamera*) m_ActiveCamera;
        PC->UpdateProjectionMatrix( );
        // PC->SetCameraFacing( glm::normalize( PC->GetCameraFacing( ) + PC->CalculateCameraRightVector( ) * 0.005F * (float) Engine::GetEngine( )->GetUserInputHandle( )->GetCursorDeltaPosition( ).first ) );
        // PC->SetCameraUpVector( glm::normalize( PC->GetCameraUpVector( ) - PC->GetCameraFacing( ) * 0.005F * (float) Engine::GetEngine( )->GetUserInputHandle( )->GetCursorDeltaPosition( ).second ) );

        SetCameraMatrix( );


        GetShader( )->SetMat4( "modelMatrix", glm::mat4( 1 ) );
        ApplyShaderUniforms( );

        m_Shader->Bind( );

        GL_CHECK( gl->Enable( GL_DEPTH_TEST ) )
        GL_CHECK( gl->DepthFunc( GL_LESS ) )
        GL_CHECK( gl->BindVertexArray( m_VAO ) )
        GL_CHECK( gl->DrawElements( GL_TRIANGLES, m_Indices.size( ), GL_UNSIGNED_INT, nullptr ) )
        GL_CHECK( gl->Disable( GL_DEPTH_TEST ) )
    }
}
