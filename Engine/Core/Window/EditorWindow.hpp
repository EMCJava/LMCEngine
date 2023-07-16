//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"
#include "Engine/Core/Concept/Concept.hpp"

class EditorWindow : public Window
{
public:
	using Window::Window;

	/*
	 *
	 * Render the editor layout
	 *
	 * */
	void
	Update() override;

	std::pair<float, float>
	GetHowReloadWindowDimensions() const;

	void
	SetPreviousFrameTexture(uint32_t TextureID);

private:
	/*
	 *
	 * Main viewport setup
	 *
	 * */
	float m_HotReloadWindowWidth{};
	float m_HotReloadWindowHeight{};
	uint32_t m_PreviousFrameTextureID{};

	/*
	 *
	 * Editor settings
	 *
	 * */
	bool m_ShowImGuiDemoWindow = false;
};
