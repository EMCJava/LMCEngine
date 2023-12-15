//
// Created by samsa on 11/21/2023.
//

#include "RigidMesh.hpp"

#include <Engine/Core/Input/Serializer/SerializerModel.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMesh.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMeshHitBox.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT_DS( RigidMesh )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( RigidMesh, RigidBody )

RigidMesh::RigidMesh( std::shared_ptr<ConceptMesh> Mesh, std::shared_ptr<Collider> C )
{
    SetMesh( std::move( Mesh ) );
    SetCollider( std::move( C ) );
}

std::shared_ptr<ConceptMesh>
RigidMesh::CreateMesh( const std::string& MeshPathStr )
{
    REQUIRED( !MeshPathStr.empty( ), throw std::runtime_error( "Mesh path cannot be empty" ); )

    auto Mesh = CreateConcept<ConceptMesh>( );
    SerializerModel::ToMesh( MeshPathStr, Mesh.get( ) );

    SetMesh( Mesh );
    return Mesh;
}

void
RigidMesh::SetMesh( std::shared_ptr<ConceptMesh> Mesh )
{
    m_Renderable = AddConcept<RenderableMesh>( std::move( Mesh ) ).Get( );
}

void
RigidMesh::SetCollider( std::shared_ptr<Collider> C )
{
    if ( m_RigidActor != nullptr )
    {
        m_RigidActor->release( );
        m_RigidActor = nullptr;
    }

    if ( m_Collider != nullptr ) m_Collider->Destroy( );
    REQUIRED( GetOwnership( m_Collider = std::move( C ) ) )

    std::shared_ptr<RenderableMeshHitBox> HitBoxFrame;
    m_RigidActor = m_Collider->GenerateActor( &HitBoxFrame );
    if ( HitBoxFrame != nullptr ) GetOwnership( HitBoxFrame );

    auto* UserData = dynamic_cast<RigidBody*>( this );
    REQUIRED( m_RigidActor && this == UserData )
    {
        m_RigidActor->userData = UserData;

        auto PhyEngine = Engine::GetEngine( )->GetPhysicsEngine( );
        PhyEngine->GetScene( )->addActor( *m_RigidActor );
    }
}
