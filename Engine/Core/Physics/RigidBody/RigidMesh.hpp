//
// Created by samsa on 11/21/2023.
//

#pragma once

#include "RigidBody.hpp"

#include <Engine/Core/Physics/Collider/ColliderMesh.hpp>

namespace physx
{
class PxMaterial;
}

class PureConceptCollider;

/*
 *
 * Contain renderable mesh and collider
 *
 * */
class RigidMesh : public RigidBody
{
    DECLARE_CONCEPT( RigidMesh, RigidBody )

public:
    template <typename Mapping = void*>
    RigidMesh( const std::string& MeshPathStr, physx::PxMaterial* Material, bool Static = false, Mapping&& ColliderMapping = nullptr )
    {
        auto Mesh = CreateMesh( MeshPathStr );

        REQUIRED( Material != nullptr, throw std::runtime_error( "Physx engine and material cannot be nullptr" ); )
        SetCollider( CreateConcept<ColliderMesh>( Mesh, Material, Static, ColliderMapping ) );
    }
    template <typename Mapping = void*>
    RigidMesh( std::shared_ptr<ConceptMesh> Mesh, physx::PxMaterial* Material, bool Static = false, Mapping&& ColliderMapping = nullptr )
    {
        SetMesh( Mesh );

        REQUIRED( Material != nullptr, throw std::runtime_error( "Physx engine and material cannot be nullptr" ); )
        SetCollider( CreateConcept<ColliderMesh>( Mesh, Material, Static, ColliderMapping ) );
    }

    RigidMesh( std::shared_ptr<ConceptMesh> Mesh, std::shared_ptr<Collider> C );

    std::shared_ptr<ConceptMesh>
    CreateMesh( const std::string& MeshPathStr );

    void
    SetMesh( std::shared_ptr<ConceptMesh> Mesh );

    void
    SetCollider( std::shared_ptr<Collider> C );

    ENABLE_IMGUI( RigidMesh )
};
