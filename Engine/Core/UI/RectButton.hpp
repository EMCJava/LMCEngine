//
// Created by samsa on 8/31/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>

#include <glm/glm.hpp>

#include <functional>

class RectButton : public ConceptApplicable
{
    DECLARE_CONCEPT( RectButton, ConceptApplicable )
private:
    void
    SetupButton( );

public:
    using CallbackSignature = void( );

    /*
     *
     * 0 for Width means auto resize to text width
     * < 0 means auto resize with additional space abs(val)
     *
     * */
    RectButton( int Width, int Height );

    virtual void
    SetEnabled( bool Enabled ) override;

    const glm::vec3&
    SetCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    void
    SetActiveCamera( class PureConceptCamera* ActiveCamera );

    void
    SetPressReactColor( const glm::vec4& Color );
    void
    SetDefaultColor( const glm::vec4& Color );
    void
    SetTextColor( const glm::vec3& Color );

    virtual void
    Apply( ) override;

    void
    SetCallback( auto&& Callback ) { m_Callback = Callback; }

    void
    SetText( const std::string& Text );

    // Valid value: [0, 1]
    void
    SetPivot( FloatTy X = 0, FloatTy Y = 0 );

protected:
    int                         m_UserDefinedWidth { }, m_UserDefinedHeight { };
    int                         m_Width { }, m_Height { };
    std::pair<FloatTy, FloatTy> m_Pivot { };
    std::string                 m_ButtonTextStr = "Button";

    std::shared_ptr<class SpriteSquare>       m_SpriteSquare;
    std::shared_ptr<class PureConceptAABBSquare> m_HitBox;
    std::shared_ptr<class Text>               m_ButtonText;

    static constexpr glm::vec4 UnknownColor { 0.9, 0.0, 0.9, 1.0 };
    glm::vec4                  m_DefaultColor { UnknownColor }, m_PressColor { UnknownColor };
    glm::vec3                  m_TextColor { 0 };

    // Used to center text in button
    std::pair<FloatTy, FloatTy> m_TextAlignmentOffset;

    std::function<CallbackSignature> m_Callback { };

    FloatTy m_PressReactTimeLeft = 0;

    ENABLE_IMGUI( RectButton )
};
