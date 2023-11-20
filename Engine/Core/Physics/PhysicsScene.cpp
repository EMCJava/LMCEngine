//
// Created by samsa on 11/21/2023.
//

#include "PhysicsScene.hpp"

#include <PxPhysicsAPI.h>

#include <spdlog/spdlog.h>

PhysicsScene::PhysicsScene( physx::PxPhysics* Physics )
    : m_Physics( Physics )
{
    physx::PxSceneDesc sceneDesc( Physics->getTolerancesScale( ) );
    sceneDesc.gravity       = physx::PxVec3( 0.0f, -9.81f, 0.0f );
    m_Dispatcher            = physx::PxDefaultCpuDispatcherCreate( 4 );
    sceneDesc.cpuDispatcher = m_Dispatcher;
    sceneDesc.filterShader  = physx::PxDefaultSimulationFilterShader;
    m_Scene                 = Physics->createScene( sceneDesc );

    // flags
    m_Scene->setFlag( physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true );

    physx::PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient( );
    if ( pvdClient )
    {
        spdlog::info( "Setting PVD flag" );
        pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
        pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
        pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
    }
}

PhysicsScene::operator physx::PxScene&( )
{
    return *m_Scene;
}
