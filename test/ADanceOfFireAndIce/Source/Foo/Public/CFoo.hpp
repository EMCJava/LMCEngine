//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>

#include <Engine/Core/Audio/AudioEngine.hpp>

class CFoo : public ConceptApplicable
{
    DECLARE_CONCEPT( CFoo, ConceptApplicable )

public:
    CFoo( );

    void
    Apply( ) override;

private:
    AudioHandle m_DelayCheckingHandle;
    AudioHandle m_MainAudioHandle;

    bool    m_IsCheckingDeviceDelay = true;
    int64_t m_UserDeviceOffsetMS { };
};