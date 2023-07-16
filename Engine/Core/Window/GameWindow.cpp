//
// Created by loyus on 7/2/2023.
//

#include "GameWindow.hpp"

#include <Engine/Engine.hpp>

#include <glad/gl.h>

void
GameWindow::Update()
{
	MakeContextCurrent();

	const auto *gl = Engine::GetEngine()->GetGLContext();

	gl->Viewport(0, 0, m_Width, m_Height);
	gl->ClearColor(1, 1, 1, 1);
	gl->Clear(GL_COLOR_BUFFER_BIT);
}
