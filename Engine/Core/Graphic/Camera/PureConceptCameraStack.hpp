//
// Created by samsa on 10/14/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConcept.hpp>

#include "PureConceptCamera.hpp"

#include <stack>

class PureConceptCameraStack : public PureConcept
{
    DECLARE_CONCEPT( PureConceptCameraStack, PureConcept )

public:
    void
    PushCamera( std::shared_ptr<PureConceptCamera> Camera );

    std::shared_ptr<PureConceptCamera>
    PopCamera( );

    void
    Clear( );

    std::shared_ptr<PureConceptCamera>
    GetCamera( ) const;

    /*
     *
     * Can throw std::runtime_error if camera stack is empty.
     *
     * */
    std::shared_ptr<PureConceptCamera>&
    GetCameraRef( );

private:
    std::stack<std::shared_ptr<PureConceptCamera>> m_CameraStack;
};
