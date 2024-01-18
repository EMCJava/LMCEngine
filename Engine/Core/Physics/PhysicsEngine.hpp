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

// Controller
class PxControllerManager;
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
    GetPhysxHandle( ) const noexcept { return m_Physics; }

    auto*
    GetCooking( ) const noexcept { return m_Cooking; }

    PhysicsScene&
    GetScene( );

    auto*
    GetControllerManager( ) const noexcept { return m_ControllerManager.get( ); }

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

    std::shared_ptr<physx::PxControllerManager> m_ControllerManager;

    physx::PxPvd* m_Pvd = nullptr;
};
