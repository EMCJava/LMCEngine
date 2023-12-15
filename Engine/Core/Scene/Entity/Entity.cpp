//
// Created by samsa on 12/15/2023.
//

#include "Entity.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <ImGui/ImGui.hpp>

DEFINE_CONCEPT_DS( Entity )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( Entity, Orientation )