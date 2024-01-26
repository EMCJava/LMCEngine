//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class GameManager : public ConceptList
{
    DECLARE_CONCEPT( GameManager, ConceptList )

public:
    GameManager( );

    void
    Apply( ) override;

    void
    TestInvokable( );

private:
    std::shared_ptr<class PureConceptPerspectiveCamera> m_MainCamera;
    std::shared_ptr<class PhyControllerEntity>          m_CharController;

    /*
     *
     * Scenes
     *
     * */
    std::shared_ptr<class ConceptList> m_WandEditorScene;

    ENABLE_IMGUI( GameManager )
};