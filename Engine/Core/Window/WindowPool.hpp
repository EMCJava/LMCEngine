//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <vector>

class WindowPool
{

public:
	WindowPool() = default;
	~WindowPool();

	class Window *
	CreateWindow(int Width, int Height, const char *Title, bool Fullscreen = false, bool Create = true);

	class Window *
	CreateWindowFullscreen(const char *Title, bool Create = true);

	void
	Update();

	bool
	PrimaryShouldClose() const;

private:
	struct ImGuiContext *m_ImGuiContext = nullptr;
	std::vector<class Window *> m_Windows;
};
