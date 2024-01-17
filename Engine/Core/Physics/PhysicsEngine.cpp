//
// Created by samsa on 11/21/2023.
//

#include "PhysicsEngine.hpp"
#include "PhysicsScene.hpp"

#include <PxPhysicsAPI.h>

#include <spdlog/spdlog.h>

PhysicsEngine::PhysicsEngine( )
{
    InitializePhysx( );
}

PhysicsEngine::~PhysicsEngine( )
{
    TerminatePhysx( );
}

void
PhysicsEngine::TerminatePhysx( )
{
    spdlog::info( "PhysX releasing!" );
}

void
PhysicsEngine::InitializePhysx( )
{
    m_DefaultAllocatorCallback = std::make_unique<physx::PxDefaultAllocator>( );
    m_DefaultErrorCallback     = std::make_unique<physx::PxDefaultErrorCallback>( );
    m_ToleranceScale           = std::make_unique<physx::PxTolerancesScale>( );

    // init physx
    m_Foundation = PxCreateFoundation( PX_PHYSICS_VERSION, *m_DefaultAllocatorCallback, *m_DefaultErrorCallback );
    if ( !m_Foundation ) throw std::runtime_error( "PxCreateFoundation failed!" );

    m_Pvd                            = PxCreatePvd( *m_Foundation );
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
    m_Pvd->connect( *transport, physx::PxPvdInstrumentationFlag::eALL );

    m_ToleranceScale->length = 1;      // typical length of an object
    m_ToleranceScale->speed  = 9.81;   // typical speed of an object, gravity*1s is a reasonable choice
    m_Physics                = PxCreatePhysics( PX_PHYSICS_VERSION, *m_Foundation, *m_ToleranceScale, true, m_Pvd );
    // mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

    m_MainScene = std::make_shared<PhysicsScene>( m_Physics );
    m_Cooking   = PxCreateCooking( PX_PHYSICS_VERSION, *m_Foundation, physx::PxCookingParams( *m_ToleranceScale ) );
    if ( !m_Cooking ) throw std::runtime_error( "PxCreateCooking failed!" );

    auto* Controller    = PxCreateControllerManager( *m_MainScene );
    m_ControllerManager = std::shared_ptr<physx::PxControllerManager>( Controller, []( auto* C ) { C->release( ); } );
}

PhysicsScene&
PhysicsEngine::GetScene( )
{
    return *m_MainScene;
}

PhysicsEngine::operator physx::PxPhysics&( )
{
    return *m_Physics;
}
