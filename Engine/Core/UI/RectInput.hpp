//
// Created by EMCJava on 4/1/2024.
//

#pragma once

#include "RectButton.hpp"

class RectInput : public RectButton
{
    DECLARE_CONCEPT( RectInput, RectButton )
public:
    RectInput( int Width, int Height );

    void
    SetCallback( std::function<CallbackSignature>&& Callback ) override { m_Callback = Callback; }

    virtual void
    Apply( ) override;

protected:
    bool m_Selected = false;

    ENABLE_IMGUI( RectInput )
};
