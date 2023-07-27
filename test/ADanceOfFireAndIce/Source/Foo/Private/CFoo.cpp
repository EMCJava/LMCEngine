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

const char *vertexTextureShaderSource = "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "layout (location = 1) in vec2 aTexCoord;\n"
                                        "out vec2 TexCoord;\n"
                                        "uniform mat4 projectionMatrix;\n"
                                        "uniform mat4 modelMatrix;\n"
                                        "void main()\n"
                                        "{\n"
                                        "   gl_Position = projectionMatrix * modelMatrix * vec4(aPos, 1.0);\n"
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

	auto SProgram = std::make_shared<ShaderProgram>();
	SProgram->Load(vertexTextureShaderSource, fragmentTextureShaderSource);

	auto S1 = std::make_shared<Shader>();
	S1->SetProgram(SProgram);
	auto *Sp1 = AddConcept<SpriteSquareTexture>(192 * 3, 108 * 3);
	Sp1->GetCoordinate().X = 192 * 1.5;
	Sp1->GetCoordinate().Y = -108 * 1.5 - 50;
	Sp1->SetShader(S1);
	Sp1->SetTexturePath("Access/Texture/cat_minimal.jpg");
	Sp1->SetupSprite();

	auto S2 = std::make_shared<Shader>();
	S2->SetProgram(SProgram);
	auto *Sp2 = AddConcept<SpriteSquareTexture>(192 * 3, 108 * 3);
	Sp1->GetCoordinate().X = 192 * 1.5;
	Sp2->GetCoordinate().Y = 108 * 1.5 + 50;
	Sp2->SetShader(S2);
	Sp2->SetTexturePath("Access/Texture/cat_minimal.jpg");
	Sp2->SetupSprite();

	spdlog::info("CFoo concept constructor returned");
}

CFoo::~CFoo()
{
	spdlog::info("CFoo concept destructor called");
}

void
CFoo::Apply()
{
	std::vector<Sprite*> Sps;
	GetConcepts(Sps);
	Sps[0]->GetCoordinate().X += Engine::GetEngine()->GetDeltaSecond();
	Sps[1]->GetCoordinate().X -= Engine::GetEngine()->GetDeltaSecond();

	spdlog::info("DeltaTime: {}, Coordinate: {}",
	             Engine::GetEngine()->GetDeltaSecond(),
	             GetConcept<Sprite>()->GetCoordinate());
}
