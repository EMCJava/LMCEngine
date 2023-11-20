//
// Created by samsa on 11/21/2023.
//

#pragma once

#include "PhysicsEngine.hpp"

class PhysicsScene
{
public:
    PhysicsScene( physx::PxPhysics* Physics );

    operator physx::PxScene&( );
    auto* operator->( ) { return m_Scene; }

private:
    physx::PxScene*                m_Scene      = nullptr;
    physx::PxDefaultCpuDispatcher* m_Dispatcher = nullptr;

    physx::PxPhysics* m_Physics = nullptr;
};