//
// Created by samsa on 11/21/2023.
//

#pragma once

#include <memory>

namespace physx
{
class PxDefaultAllocator;
class PxDefaultErrorCallback;
class PxDefaultCpuDispatcher;
class PxTolerancesScale;
class PxCooking;
class PxFoundation;
class PxPhysics;
class PxScene;
class PxPvd;
}   // namespace physx

class PhysicsScene;
class PhysicsEngine
{
    void
    TerminatePhysx( );

    void
    InitializePhysx( );

public:
    PhysicsEngine( );

    ~PhysicsEngine( );

    auto*
    GetPhysxHandle( ) { return m_Physics; }

    auto*
    GetCooking( ) { return m_Cooking; }

    PhysicsScene&
    GetScene( );

    operator physx::PxPhysics&( );
    auto* operator->( ) { return GetPhysxHandle( ); }

private:
    std::unique_ptr<physx::PxDefaultAllocator>     m_DefaultAllocatorCallback;
    std::unique_ptr<physx::PxDefaultErrorCallback> m_DefaultErrorCallback;
    std::unique_ptr<physx::PxTolerancesScale>      m_ToleranceScale;

    physx::PxCooking*    m_Cooking    = nullptr;
    physx::PxFoundation* m_Foundation = nullptr;
    physx::PxPhysics*    m_Physics    = nullptr;

    std::shared_ptr<PhysicsScene> m_MainScene;

    physx::PxPvd* m_Pvd = nullptr;
};
