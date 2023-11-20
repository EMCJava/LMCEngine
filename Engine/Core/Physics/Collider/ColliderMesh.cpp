//
// Created by samsa on 11/21/2023.
//

#include "ColliderMesh.hpp"

#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT_DS( ColliderMesh )

ColliderMesh::ColliderMesh( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static )
    : PureConceptCollider( PhyEngine )
{
    SetGroupMeshCollider( GroupMeshCollider, Material, Static );
}

void
ColliderMesh::SetGroupMeshCollider( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, physx::PxMaterial* Material, bool Static )
{
    m_GroupMeshCollider = GroupMeshCollider;
    REQUIRED( m_GroupMeshCollider->HasData( ), throw std::runtime_error( "GroupMeshCollider has no data" ) )

    if ( m_RigidActor != nullptr )
    {
        m_RigidActor->release( );
        m_RigidActor = nullptr;
    }

    if ( Static )
        m_RigidActor = m_GroupMeshCollider->CreateStaticRigidBodyFromCacheGroup( *Material );
    else
        m_RigidActor = m_GroupMeshCollider->CreateDynamicRigidBodyFromCacheGroup( *Material );

    REQUIRED( this == dynamic_cast<PureConceptCollider*>( this ) )
    // FIXME: should set by rigid body
    m_RigidActor->userData = dynamic_cast<PureConceptCollider*>( this );
    m_PhyEngine->GetScene( )->addActor( *m_RigidActor );
}
