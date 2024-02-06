//
// Created by samsa on 10/23/2023.
//

#include "ConceptMesh.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Input/Serializer/SerializerModel.hpp>

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

DEFINE_CONCEPT_DS( ConceptMesh )
DEFINE_SIMPLE_IMGUI_TYPE( GLBufferHandle, VAO, VBO, EBO )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ConceptMesh, PureConcept, m_GLBufferHandle )

GLBufferHandle::~GLBufferHandle( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    if ( VAO != 0 ) gl->DeleteVertexArrays( 1, &VAO );
    if ( VBO != 0 ) gl->DeleteBuffers( 1, &VBO );
    if ( EBO != 0 ) gl->DeleteBuffers( 1, &EBO );

    VAO = VBO = EBO = 0;
}

ConceptMesh::ConceptMesh( const std::string& MeshFilePath, const uint32_t Features )
{
    SetVertexFeature( (MeshVertexFeature) Features );
    SerializerModel::ToMesh( MeshFilePath, this );
}
