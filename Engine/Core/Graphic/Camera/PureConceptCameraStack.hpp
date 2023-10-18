//
// Created by samsa on 10/14/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConcept.hpp>

#include <stack>

class PureConceptCameraStack : public PureConcept
{
    DECLARE_CONCEPT( PureConceptCameraStack, PureConcept )

public:
    void
    PushCamera( std::shared_ptr<class PureConceptCamera> Camera );

    std::shared_ptr<class PureConceptCamera>
    PopCamera( );

    void
    Clear( );

    std::shared_ptr<class PureConceptCamera>
    GetCamera( ) const;

    /*
     *
     * Can throw std::runtime_error if camera stack is empty.
     *
     * */
    std::shared_ptr<class PureConceptCamera>&
    GetCameraRef( );

private:
    std::stack<std::shared_ptr<class PureConceptCamera>> m_CameraStack;
};
