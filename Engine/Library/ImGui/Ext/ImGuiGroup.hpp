//
// Created by loyus on 8/22/2023.
//

#pragma once

#include <Engine/Engine.hpp>

#include <ImGui/ImGui.hpp>

// https://github.com/ocornut/imgui/issues/1496
namespace ImGuiGroup
{
inline void
BeginGroupPanel( const char* name, const ImVec2& size = ImVec2 { -1, 0 } )
{
    ImGui::BeginGroup( );

    auto itemSpacing = ImGui::GetStyle( ).ItemSpacing;
    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0.0f, 0.0f ) );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );

    auto frameHeight = ImGui::GetFrameHeight( );
    ImGui::BeginGroup( );

    ImVec2 effectiveSize = size;
    if ( size.x < 0.0f )
        effectiveSize.x = ImGui::GetContentRegionAvail( ).x;
    else
        effectiveSize.x = size.x;
    ImGui::Dummy( ImVec2( effectiveSize.x, 0.0f ) );

    ImGui::Dummy( ImVec2( frameHeight * 0.5f, 0.0f ) );
    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::BeginGroup( );
    ImGui::Dummy( ImVec2( frameHeight * 0.5f, 0.0f ) );
    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::TextUnformatted( name );
    auto labelMin = ImGui::GetItemRectMin( );
    auto labelMax = ImGui::GetItemRectMax( );
    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::Dummy( ImVec2( 0.0, frameHeight + itemSpacing.y ) );
    ImGui::BeginGroup( );

    ImGui::PopStyleVar( 2 );

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow( )->ContentRegionRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->WorkRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->InnerRect.Max.x -= frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow( )->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow( )->Size.x -= frameHeight;

    auto itemWidth = ImGui::CalcItemWidth( );
    ImGui::PushItemWidth( ImMax( 0.0f, itemWidth - frameHeight ) );

    Engine::GetEngine( )->GetImGuiGroupPanelLabelStack( )->push_back( ImRect( labelMin, labelMax ) );
}

inline void
EndGroupPanel( )
{
    ImGui::PopItemWidth( );

    auto itemSpacing = ImGui::GetStyle( ).ItemSpacing;

    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0.0f, 0.0f ) );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );

    auto frameHeight = ImGui::GetFrameHeight( );

    ImGui::EndGroup( );

    // ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

    ImGui::EndGroup( );

    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::Dummy( ImVec2( frameHeight * 0.5f, 0.0f ) );
    ImGui::Dummy( ImVec2( 0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y ) );

    ImGui::EndGroup( );

    auto itemMin = ImGui::GetItemRectMin( );
    auto itemMax = ImGui::GetItemRectMax( );
    // ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

    auto* ImGuiGroupPanelLabelStack = Engine::GetEngine( )->GetImGuiGroupPanelLabelStack( );
    auto  labelRect                 = ImGuiGroupPanelLabelStack->back( );
    ImGuiGroupPanelLabelStack->pop_back( );

    ImVec2 halfFrame = ImVec2( frameHeight * 0.25f, frameHeight ) * 0.5f;
    ImRect frameRect = ImRect( itemMin + halfFrame, itemMax - ImVec2( halfFrame.x, 0.0f ) );
    labelRect.Min.x -= itemSpacing.x;
    labelRect.Max.x += itemSpacing.x;
    for ( int i = 0; i < 4; ++i )
    {
        switch ( i )
        {
        // left half-plane
        case 0: ImGui::PushClipRect( ImVec2( -FLT_MAX, -FLT_MAX ), ImVec2( labelRect.Min.x, FLT_MAX ), true ); break;
        // right half-plane
        case 1: ImGui::PushClipRect( ImVec2( labelRect.Max.x, -FLT_MAX ), ImVec2( FLT_MAX, FLT_MAX ), true ); break;
        // top
        case 2: ImGui::PushClipRect( ImVec2( labelRect.Min.x, -FLT_MAX ), ImVec2( labelRect.Max.x, labelRect.Min.y ), true ); break;
        // bottom
        case 3: ImGui::PushClipRect( ImVec2( labelRect.Min.x, labelRect.Max.y ), ImVec2( labelRect.Max.x, FLT_MAX ), true ); break;
        }

        ImGui::GetWindowDrawList( )->AddRect(
            frameRect.Min, frameRect.Max,
            ImColor( ImGui::GetStyleColorVec4( ImGuiCol_Border ) ),
            halfFrame.x );

        ImGui::PopClipRect( );
    }

    ImGui::PopStyleVar( 2 );

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow( )->ContentRegionRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->WorkRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->InnerRect.Max.x += frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow( )->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow( )->Size.x += frameHeight;

    ImGui::Dummy( ImVec2( 0.0f, 0.0f ) );

    ImGui::EndGroup( );
}
}   // namespace ImGuiGroup