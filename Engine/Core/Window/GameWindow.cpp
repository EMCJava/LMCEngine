//
// Created by loyus on 7/2/2023.
//

#include "GameWindow.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

void
GameWindow::Update()
{
	MakeContextCurrent();

	glViewport(0, 0, m_Width, m_Height);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}
