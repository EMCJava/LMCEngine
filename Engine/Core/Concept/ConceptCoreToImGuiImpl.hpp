//
// Created by loyus on 8/12/2023.
//

#pragma once

#include <memory>

#include <imgui.h>

inline void
ToImGuiWidget( const char* Name, uint32_t* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_S32, Value );
}

inline void
ToImGuiWidget( const char* Name, float* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_Float, Value );
}

//void
//ToImGuiWidget( const char* Name, std::shared_ptr<Shader>* Value )
//{
//    auto PtrAddress = reinterpret_cast<uint64_t>( Value->get( ) );
//    ImGui::BeginDisabled( );
//    ImGui::InputScalar( Name, ImGuiDataType_U64, &PtrAddress, nullptr, nullptr, "%p", ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal );
//    ImGui::EndDisabled( );
//}
