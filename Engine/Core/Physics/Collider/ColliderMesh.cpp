//
// Created by samsa on 11/21/2023.
//

#include "ColliderMesh.hpp"

#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMeshHitBox.hpp>
#include <utility>

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT_DS( ColliderMesh )

ColliderMesh::ColliderMesh( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, physx::PxMaterial* Material, bool Static )
{
    SetGroupMeshCollider( std::move( GroupMeshCollider ), Material, Static );
}

void
ColliderMesh::SetGroupMeshCollider( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, physx::PxMaterial* Material, bool Static )
{
    m_GroupMeshCollider = std::move( GroupMeshCollider );
    REQUIRED( m_GroupMeshCollider->HasData( ), throw std::runtime_error( "GroupMeshCollider has no data" ) )
    m_ColliderIsStatic = Static;
    m_Material         = Material;
}

physx::PxRigidActor*
ColliderMesh::GenerateActor( std::shared_ptr<RenderableMeshHitBox>* HitBoxFrame )
{
    physx::PxRigidActor* Result = nullptr;

    if ( m_ColliderIsStatic )
        Result = m_GroupMeshCollider->CreateStaticRigidBodyFromCacheGroup( *m_Material, HitBoxFrame );
    else
        Result = m_GroupMeshCollider->CreateDynamicRigidBodyFromCacheGroup( *m_Material, HitBoxFrame );

    return Result;
}
