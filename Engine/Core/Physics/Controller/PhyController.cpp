//
// Created by Lo Yu Sum on 2024/01/17.
//

#include "PhyController.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>

#include <PxPhysicsAPI.h>

#include <spdlog/spdlog.h>

// For m_CapsuleDesc
PhyController::PhyController( )
{ }

void
PhyController::CreateController( const glm::vec3& position, FloatTy Height, FloatTy Radius, physx::PxMaterial* Material )
{
    m_CapsuleDesc                   = std::make_unique<physx::PxCapsuleControllerDesc>( );
    m_CapsuleDesc->material         = Material;
    m_CapsuleDesc->position         = { position.x, position.y, position.z };
    m_CapsuleDesc->height           = Height;
    m_CapsuleDesc->radius           = Radius;
    m_CapsuleDesc->slopeLimit       = 0.0f;
    m_CapsuleDesc->contactOffset    = 0.1f;
    m_CapsuleDesc->stepOffset       = 0.02f;
    m_CapsuleDesc->reportCallback   = nullptr;
    m_CapsuleDesc->behaviorCallback = nullptr;

    auto* ControllerManager = Engine::GetEngine( )->GetPhysicsEngine( )->GetControllerManager( );
    auto* Controller        = ControllerManager->createController( *m_CapsuleDesc );
    // FIXME: type info for dynamic_cast on UNIX platform
    m_Controller            = static_cast<physx::PxCapsuleController*>( Controller );

    // remove controller shape from scene query avoid any raycast hit
    physx::PxRigidDynamic* actor = m_Controller->getActor( );
    if ( actor )
    {
        if ( actor->getNbShapes( ) )
        {
            physx::PxShape* ctrlShape;
            actor->getShapes( &ctrlShape, 1 );
            ctrlShape->setFlag( physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false );
        } else
            spdlog::error( "character actor has no shape" );
    } else
    {
        spdlog::error( "character could not create actor" );
    }
}

physx::PxControllerCollisionFlags
PhyController::MoveRel( const glm::vec3& Displacement, FloatTy DeltaTime )
{
    REQUIRED_IF( m_Controller != nullptr )
    {
        auto Lock = Engine::GetEngine( )->GetPhysicsThreadLock( );
        return m_Controller->move( *( (physx::PxVec3*) &Displacement ), 0.0f, DeltaTime, physx::PxControllerFilters( ) );
    }
}

glm::dvec3
PhyController::GetFootPosition( )
{
    REQUIRED_IF( m_Controller != nullptr )
    {
        static_assert( sizeof( decltype( m_Controller->getFootPosition( ) ) ) == sizeof( glm::dvec3 ) );
        physx::PxExtendedVec3 pxVec3 = m_Controller->getFootPosition( );
        return reinterpret_cast<glm::dvec3&>( pxVec3 );
    }

    return glm::vec3 { };
}

PhyController::~PhyController( )
{
    if ( m_Controller != nullptr )
    {
        m_Controller->release( );
        m_Controller = nullptr;
    }
}
