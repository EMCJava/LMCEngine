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

	/*
	 *
	 * Dynamically allocates a new window with type WindowTy, append to list
	 *
	 * */
	template<typename WindowTy>
	WindowTy *
	CreateWindow(int Width, int Height, const char *Title, bool Fullscreen = false, bool Create = true);

	/*
	 *
	 * Dynamically allocates a new window with type WindowTy, append to list
	 *
	 * */
	template<typename WindowTy>
	WindowTy *
	CreateWindowFullscreen(const char *Title, bool Create = true);

	/*
	 *
	 * Get primary window size
	 *
	 * */
	std::pair<int, int>
	GetPrimaryMonitorMode() const;

	/*
	 *
	 * Update all window in list
	 *
	 * */
	void
	Update();

	/*
	 *
	 * Return true if window list is empty
	 *
	 * */
	bool
	ShouldClose() const;

private:
	std::vector<class Window *> m_Windows;
};

#include "WindowPool.inl"