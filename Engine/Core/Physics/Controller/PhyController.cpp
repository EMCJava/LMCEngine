//
// Created by Lo Yu Sum on 2024/01/17.
//

#include "PhyController.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>

#include <PxPhysicsAPI.h>

#include <spdlog/spdlog.h>

void
PhyController::CreateController( const glm::vec3& position, FloatTy Height, FloatTy Radius, physx::PxMaterial* Material )
{
    physx::PxCapsuleControllerDesc CapsuleDesc;
    CapsuleDesc.material         = Material;
    CapsuleDesc.position         = { position.x, position.y, position.z };
    CapsuleDesc.height           = Height;
    CapsuleDesc.radius           = Radius;
    CapsuleDesc.slopeLimit       = 0.0f;
    CapsuleDesc.contactOffset    = 0.1f;
    CapsuleDesc.stepOffset       = 0.02f;
    CapsuleDesc.reportCallback   = nullptr;
    CapsuleDesc.behaviorCallback = nullptr;

    auto* ControllerManager = Engine::GetEngine( )->GetPhysicsEngine( )->GetControllerManager( );
    auto* Controller        = ControllerManager->createController( CapsuleDesc );
    m_Controller            = static_cast<physx::PxCapsuleController*>( Controller );

    // remove controller shape from scene query for standup overlap test
    return;
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

void
PhyController::MoveRel( const glm::vec3& Velocity, FloatTy DeltaTime )
{
    REQUIRED_IF( m_Controller != nullptr )
    {
        m_Controller->move( *( (physx::PxVec3*) &Velocity ), 0.0f, DeltaTime, physx::PxControllerFilters( ) );
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
