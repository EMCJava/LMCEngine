//
// Created by samsa on 11/21/2023.
//

#include "RigidMesh.hpp"

#include <Engine/Core/Input/Serializer/SerializerModel.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMesh.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMeshHitBox.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMeshCluster.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>
#include <Engine/Core/Scene/Entity/EntityRenderable.hpp>

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT_DS( RigidMesh )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( RigidMesh, RigidBody )

RigidMesh::RigidMesh( std::shared_ptr<ConceptMesh> Mesh, std::shared_ptr<Collider> C )
{
    m_Renderable = CreateConcept<RenderableMesh>( std::move( Mesh ) );
    AddConcept<EntityRenderable>( m_Renderable );
    SetCollider( std::move( C ) );
}

std::shared_ptr<RenderableMeshCluster>
RigidMesh::LoadRenderableMesh( const std::string& MeshPathStr )
{
    REQUIRED( !MeshPathStr.empty( ), throw std::runtime_error( "Mesh path cannot be empty" ); )

    m_Renderable = SerializerModel::ToMeshCluster( MeshPathStr, eFeatureDefault );
    AddConcept<EntityRenderable>( m_Renderable );
    return std::dynamic_pointer_cast<RenderableMeshCluster>( m_Renderable );
}

void
RigidMesh::SetCollider( std::shared_ptr<Collider> C )
{
    if ( m_RigidActor != nullptr )
    {
        m_RigidActor = nullptr;

        Engine::GetEngine( )->AddPhysicsCallback( [ Actor = m_RigidActor ]( auto* PhyEngine ) {
            PhyEngine->GetScene( )->removeActor( *Actor );
            Actor->release( );
        } );
    }

    if ( m_Collider != nullptr ) m_Collider->Destroy( );
    REQUIRED( GetOwnership( m_Collider = std::move( C ) ) )

    std::shared_ptr<RenderableMeshHitBox> HitBoxFrame;
    m_RigidActor = m_Collider->GenerateActor( &HitBoxFrame );
    if ( HitBoxFrame != nullptr ) AddConcept<EntityRenderable>( HitBoxFrame );

    auto* UserData = dynamic_cast<RigidBody*>( this );
    REQUIRED( m_RigidActor && this == UserData )
    {
        m_RigidActor->userData = UserData;

        Engine::GetEngine( )->AddPhysicsCallback( [ Actor = m_RigidActor ]( auto* PhyEngine ) {
            PhyEngine->GetScene( )->addActor( *Actor );
        } );
    }
}
