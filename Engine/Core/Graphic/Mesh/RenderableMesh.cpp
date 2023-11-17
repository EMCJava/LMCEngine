//
// Created by samsa on 11/18/2023.
//

#include "ConceptMesh.hpp"
#include "RenderableMesh.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <utility>

DEFINE_CONCEPT_DS( RenderableMesh )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( RenderableMesh, ConceptRenderable, m_ConceptMesh )

void
RenderableMesh::Render( )
{
    REQUIRED( m_ConceptMesh != nullptr, return )

    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    m_Shader->Bind( );
    SetCameraMatrix( );

    GetShader( )->SetMat4( "modelMatrix", GetModelMatrix( ) );
    ApplyShaderUniforms( );

    GL_CHECK( gl->Enable( GL_DEPTH_TEST ) )
    GL_CHECK( gl->DepthFunc( GL_LESS ) )
    GL_CHECK( gl->BindVertexArray( m_ConceptMesh->GetGLBufferHandle( ).VAO ) )
    GL_CHECK( gl->DrawElements( GL_TRIANGLES, m_ConceptMesh->GetIndices( ).size( ), GL_UNSIGNED_INT, nullptr ) )
    GL_CHECK( gl->Disable( GL_DEPTH_TEST ) )
}

RenderableMesh::RenderableMesh( std::shared_ptr<ConceptMesh> Mesh )
{
    SetStaticMesh( std::move( Mesh ) );
}

void
RenderableMesh::SetStaticMesh( std::shared_ptr<ConceptMesh> Mesh )
{
    m_ConceptMesh = std::move( Mesh );
}
