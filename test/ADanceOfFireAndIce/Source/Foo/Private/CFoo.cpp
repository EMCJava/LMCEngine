//
// Created by samsa on 7/8/2023.
//

#include "CFoo.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/PureConceptCoordinate.hpp>
#include <Engine/Core/Concept/ConceptRenderable.hpp>

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquare.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_MA_SE(CFoo, Concept)

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "uniform mat4 projectionMatrix;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = projectionMatrix * vec4(aPos, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

CFoo::CFoo()
{
	spdlog::info("CFoo concept constructor called");

	auto S = std::make_shared<Shader>();
	S->Load(vertexShaderSource, fragmentShaderSource);
	AddConcept<SpriteSquare>(100, 100)->SetShader(S);

	spdlog::info("CFoo concept constructor returned");
}

CFoo::~CFoo()
{
	spdlog::info("CFoo concept destructor called");
}

void
CFoo::Apply()
{
	spdlog::info("DeltaTime: {}, Coordinate: {}",
	             Engine::GetEngine()->GetDeltaSecond(),
	             GetConcept<Sprite>()->GetCoordinate());
}
