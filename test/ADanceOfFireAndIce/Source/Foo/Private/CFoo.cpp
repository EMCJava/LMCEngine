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
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>

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

const char *vertexTextureShaderSource = "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "layout (location = 1) in vec2 aTexCoord;\n"
                                        "out vec2 TexCoord;\n"
                                        "uniform mat4 projectionMatrix;\n"
                                        "void main()\n"
                                        "{\n"
                                        "   gl_Position = projectionMatrix * vec4(aPos, 1.0);\n"
                                        "   TexCoord = aTexCoord;\n"
                                        "}\0";
const char *fragmentTextureShaderSource = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "in vec2 TexCoord;\n"
                                          "uniform sampler2D sample_texture;\n"
                                          "void main()\n"
                                          "{\n"
                                          "   FragColor = mix(texture(sample_texture, TexCoord), vec4(1.0f, 0.5f, 0.2f, 1.0f), 0.2);\n"
                                          "}\n\0";

CFoo::CFoo()
{
	spdlog::info("CFoo concept constructor called");

	auto S = std::make_shared<Shader>();
	S->Load(vertexTextureShaderSource, fragmentTextureShaderSource);
	auto *Sp = AddConcept<SpriteSquareTexture>(192 * 3, 108 * 3);
	Sp->SetShader(S);
	Sp->SetTexturePath("Access/Texture/cat_minimal.jpg");
	Sp->SetupSprite();

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
