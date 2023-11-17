//
// Created by samsa on 10/23/2023.
//

#include "ConceptMesh.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT_DS( ConceptMesh )
DEFINE_SIMPLE_IMGUI_TYPE( GLBufferHandle, VAO, VBO, EBO )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ConceptMesh, PureConcept, m_GLBufferHandle )
