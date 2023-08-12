//
// Created by loyus on 8/12/2023.
//

#pragma once

#include <Engine/Core/Scene/Orientation/OrientationCoordinate.hpp>

#include <memory>

#include <imgui.h>

inline void
ToImGuiWidget( const char* Name, OrientationCoordinate::Coordinate* Value )
{
    static_assert( sizeof( OrientationCoordinate::Coordinate ) == sizeof( float ) * 3 );
    static_assert( std::is_same_v<FloatTy, float> );
    ImGui::InputFloat3( Name, (FloatTy*) Value );
}

inline void
ToImGuiWidget( const char* Name, size_t* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_U64, Value );
}

inline void
ToImGuiWidget( const char* Name, uint32_t* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_U32, Value );
}

inline void
ToImGuiWidget( const char* Name, int32_t* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_S32, Value );
}

inline void
ToImGuiWidget( const char* Name, float* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_Float, Value );
}

inline void
ToImGuiWidget( const char* Name, std::string* Value )
{
    Value->reserve( Value->size( ) * 2 );
    ImGui::InputText( Name, Value->data( ), Value->capacity( ) + 1 );
}

template <typename Ty>
void
ToImGuiWidget( const char* Name, std::shared_ptr<Ty>* Value )
{
    auto PtrAddress = reinterpret_cast<uint64_t>( Value->get( ) );
    ImGui::BeginDisabled( );
    ImGui::InputScalar( Name, ImGuiDataType_U64, &PtrAddress, nullptr, nullptr, "%p", ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal );
    ImGui::EndDisabled( );
}

/*
 *
 * Is a double pointer
 *
 * */
template <typename Ty>
inline typename std::enable_if_t<std::is_pointer<typename std::remove_pointer<Ty>::type>::value, void>
ToImGuiPointerSwitch( const char* Name, Ty Value )
{
    ImGui::BeginDisabled( );
    ImGui::InputScalar( Name, ImGuiDataType_U64, (void*) Value, nullptr, nullptr, "%p", ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal );
    ImGui::EndDisabled( );
}

/*
 *
 * Is a pointer
 *
 * */
template <typename Ty>
inline typename std::enable_if_t<!std::is_pointer<typename std::remove_pointer<Ty>::type>::value, void>
ToImGuiPointerSwitch( const char* Name, Ty Value )
{
    ToImGuiWidget( Name, Value );
}