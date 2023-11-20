//
// Created by samsa on 11/21/2023.
//

#pragma once

#include "PureConceptCollider.hpp"
#include "Serializer/ColliderSerializerGroupMesh.hpp"

/*
 *
 * PureConceptCollider from meshes, should not be shared
 *
 * */
class ColliderMesh : public PureConceptCollider
{
    DECLARE_CONCEPT( ColliderMesh, PureConceptCollider )

public:
    /*
     *
     * This will generate a new collider for the mesh
     *
     * */
    template <typename Mapping = void*>
    ColliderMesh( std::shared_ptr<class ConceptMesh> StaticMesh, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static = false, Mapping&& ColliderMapping = nullptr )
        : ColliderMesh( std::make_shared<ColliderSerializerGroupMesh>( StaticMesh, PhyEngine, ColliderMapping ), PhyEngine, Material, Static )
    { }

    ColliderMesh( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static = false );

    /*
     *
     * GroupMeshCollider need to be loaded and initialized
     *
     * */
    void
    SetGroupMeshCollider( std::shared_ptr<ColliderSerializerGroupMesh> GroupMeshCollider, physx::PxMaterial* Material, bool Static );

private:
    /*
     *
     * PureConceptCollider
     *
     * */
    std::shared_ptr<ColliderSerializerGroupMesh> m_GroupMeshCollider;
};
