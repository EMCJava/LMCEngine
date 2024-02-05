//
// Created by Lo Yu Sum on 2024/01/25.
//


#pragma once

#include "Entity.hpp"

class EntityRenderable : public Entity
{
    DECLARE_CONCEPT( EntityRenderable, Entity )

public:
    EntityRenderable( std::shared_ptr<class ConceptRenderable> RenderableConcept );

    void
    Apply( ) override;

    ENABLE_IMGUI( EntityRenderable )
};
