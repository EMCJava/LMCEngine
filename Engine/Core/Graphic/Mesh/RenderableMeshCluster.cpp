//
// Created by samsa on 2/2/2024.
//

#include "RenderableMeshCluster.hpp"
#include "RenderableMesh.hpp"
#include "ConceptMesh.hpp"

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

DEFINE_CONCEPT_DS( RenderableMeshCluster )

RenderableMeshCluster::RenderableMeshCluster( )
{
    SetSearchThrough( false );
}

void
RenderableMeshCluster::Render( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    GetConcepts( m_RenderableMeshes );

    m_Shader->Bind( );
    SetCameraMatrix( );
    ApplyShaderUniforms( );

    m_RenderableMeshes.ForEachOriginal( [ gl ]( std::shared_ptr<RenderableMesh>& RM ) {
        const auto& Material = RM->GetMaterial( );
        if ( Material != nullptr ) Material->ActivateMaterial( );

        const auto& TargetMesh = RM->GetStaticMesh( );

        GL_CHECK( gl->Enable( GL_DEPTH_TEST ) )
        GL_CHECK( gl->DepthFunc( GL_LESS ) )
        GL_CHECK( gl->BindVertexArray( TargetMesh->GetGLBufferHandle( ).VAO ) )
        GL_CHECK( gl->DrawElements( GL_TRIANGLES, TargetMesh->GetIndices( ).size( ), GL_UNSIGNED_INT, nullptr ) )
        GL_CHECK( gl->Disable( GL_DEPTH_TEST ) );
    } );
}
