//
// Created by samsa on 11/21/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>

class PureConceptPerspectiveCamera;
class FirstPersonCameraController : public ConceptApplicable
{
    DECLARE_CONCEPT( FirstPersonCameraController, ConceptApplicable )

public:
    explicit FirstPersonCameraController( std::shared_ptr<PureConceptPerspectiveCamera> Camera );

    void
    Apply( ) override;

    void
    SetFreeCamera( bool Free ) noexcept { m_FreeCamera = Free; }

    auto&
    GetCamera( ) const noexcept { return m_Camera; }

    void
    SetViewSensitivity( FloatTy Sensitivity ) noexcept { m_ViewControlSensitivity = Sensitivity; }

protected:
    std::shared_ptr<PureConceptPerspectiveCamera> m_Camera;
    FloatTy                                       m_ViewControlSensitivity { 1.0f };

    /*
     *
     * Be able to fly around
     *
     * */
    bool m_FreeCamera = true;

    bool m_MouseLocked { true };

    ENABLE_IMGUI( FirstPersonCameraController )
};