//
// Created by Lo Yu Sum on 2024/01/26.
//


#pragma once

#include "PhyControllerEntity.hpp"

#include <Engine/Core/Concept/ConceptList.hpp>

class PhyControllerEntityPlayer : public PhyControllerEntity
{
    DECLARE_CONCEPT( PhyControllerEntityPlayer, PhyControllerEntity )
public:
    PhyControllerEntityPlayer( std::shared_ptr<class PureConceptPerspectiveCamera> Camera );

    void
    Apply( ) override;

protected:
    std::shared_ptr<class FirstPersonCameraController> m_CameraController;
};
