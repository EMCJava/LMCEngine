//
// Created by samsa on 11/21/2023.
//

#pragma once

#include "Collider.hpp"
#include "Serializer/ColliderSerializerGroupMesh.hpp"

/*
 *
 * PureConceptCollider from meshes, should not be shared
 *
 * */
class ColliderMesh : public Collider
{
    DECLARE_CONCEPT( ColliderMesh, Collider )

public:
    /*
     *
     * This will generate a new collider for the mesh
     *
     * */
    template <typename Mapping = void*>
    ColliderMesh( std::shared_ptr<class ConceptMesh> StaticMesh, physx::PxMaterial* Material, bool Static = false, Mapping&& ColliderMapping = nullptr )
        : ColliderMesh( std::make_shared<ColliderSerializerGroupMesh>( StaticMesh, ColliderMapping ), Material, Static )
    { }

    ColliderMesh( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, physx::PxMaterial* Material, bool Static = false );

    /*
     *
     * GroupMeshCollider need to be loaded and initialized
     *
     * */
    void
    SetGroupMeshCollider( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, physx::PxMaterial* Material, bool Static );


    physx::PxRigidActor*
    GenerateActor( std::shared_ptr<RenderableMeshHitBox>* HitBoxFrame ) override;

private:
    bool               m_ColliderIsStatic = false;
    physx::PxMaterial* m_Material         = nullptr;

    /*
     *
     * PureConceptCollider
     *
     * */
    std::shared_ptr<ColliderSerializerGroupMesh> m_GroupMeshCollider;
};
