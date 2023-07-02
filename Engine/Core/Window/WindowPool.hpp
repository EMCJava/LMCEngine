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

	template<typename WindowTy>
	WindowTy *
	CreateWindow(int Width, int Height, const char *Title, bool Fullscreen = false, bool Create = true);

	template<typename WindowTy>
	WindowTy *
	CreateWindowFullscreen(const char *Title, bool Create = true);

	std::pair<int, int>
	GetPrimaryMonitorMode() const;

	void
	Update();

	bool
	ShouldClose() const;

private:
	std::vector<class Window *> m_Windows;
};

#include "WindowPool.inl"