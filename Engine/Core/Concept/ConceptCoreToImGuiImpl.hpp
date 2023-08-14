//
// Created by loyus on 8/12/2023.
//

#pragma once

#include <Engine/Core/Scene/Orientation/OrientationCoordinate.hpp>

#include <memory>
#include <type_traits>

#include <imgui.h>

/*
 *
 * Is a pointer switches
 *
 * */
template <typename Ty>
inline typename std::enable_if_t<std::is_pointer_v<std::remove_pointer_t<Ty>>, void>
ToImGuiPointerSwitch( const char* Name, Ty Value );
template <typename Ty>
inline typename std::enable_if_t<!std::is_pointer_v<std::remove_pointer_t<Ty>> && std::is_pointer_v<Ty>, void>
ToImGuiPointerSwitch( const char* Name, Ty Value );
template <typename Ty>
inline typename std::enable_if_t<!std::is_pointer_v<Ty>, void>
ToImGuiPointerSwitch( const char* Name, Ty& Value );

inline void
ToImGuiWidget( const char* Name, OrientationCoordinate::Coordinate* Value )
{
    static_assert( sizeof( OrientationCoordinate::Coordinate ) == sizeof( float ) * 3 );
    static_assert( std::is_same_v<FloatTy, float> );
    ImGui::InputFloat3( Name, (FloatTy*) Value );
}

inline void
ToImGuiWidget( const char* Name, bool* Value )
{
    ImGui::Checkbox( Name, Value );
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

/*
 *
 * std containers
 *
 * */
template <typename Ty1, typename Ty2>
void
ToImGuiWidget( const char* Name, std::pair<Ty1, Ty2>* Value )
{
    std::string NameStr = Name;
    ToImGuiPointerSwitch( ( NameStr + ".first" ).c_str( ), &Value->first );
    ImGui::SameLine( );
    ToImGuiPointerSwitch( ( NameStr + ".second" ).c_str( ), &Value->second );
}

template <typename Ty>
void
ToImGuiWidget( const char* Name, std::weak_ptr<Ty>* Value )
{
    if ( Value->expired( ) )
    {
        ImGui::TextDisabled( "%s [ REF EXPIRED ]", Name );
    }

    ToImGuiPointerSwitch( Name, Value->lock( ).get( ) );
}

template <typename Ty>
void
ToImGuiWidget( const char* Name, std::shared_ptr<Ty>* Value )
{
    ToImGuiPointerSwitch( Name, Value->get( ) );
}

template <typename Ty>
void
ToImGuiWidget( const char* Name, std::unique_ptr<Ty>* Value )
{
    ToImGuiPointerSwitch( Name, Value->get( ) );
}

/*
 *
 * Container such as map
 *
 * */
template <template <typename, typename, typename, typename> class Container,
          typename KTy,
          typename VTy,
          typename Comparator = std::less<KTy>,
          typename Allocator  = std::allocator<std::pair<KTy, VTy>>>
void
ToImGuiWidget( const char* Name, Container<KTy, VTy, Comparator, Allocator>* Value )
{
    // FIXME: put this in engine or anything other than here
    static std::map<void*, VTy*> IndexValueSaves;
    VTy*&                        ThisValueSaves = IndexValueSaves[ Value ];

    if ( ImGui::CollapsingHeader( Name, ImGuiTreeNodeFlags_OpenOnArrow ) )
    {
        ImGui::PushID( Value );

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 5.0f );
        ImGui::BeginChild( "KVView", ImVec2( -FLT_MIN, 260 ), true, ImGuiWindowFlags_None );

        const float Width_5          = ( ImGui::GetContentRegionAvail( ).x - ImGui::GetStyle( ).ItemSpacing.x ) / 8;
        const float KeyWindowWidth   = Width_5 * 2;
        const float ValueWindowWidth = Width_5 * 6;

        VTy* SelectedValue = nullptr;

        // Keys
        if ( ImGui::BeginListBox( "##Keys", ImVec2( KeyWindowWidth, -FLT_MIN ) ) )
        {
            size_t Index = 0;
            for ( auto& KVPair : *Value )
            {
                const bool is_selected = ( ThisValueSaves == &KVPair.second );
                if ( ImGui::Selectable( std::to_string( KVPair.first ).c_str( ), is_selected ) )
                {
                    ThisValueSaves = &KVPair.second;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if ( is_selected )
                {
                    ImGui::SetItemDefaultFocus( );
                }

                Index += 1;
            }

            ImGui::EndListBox( );
        }

        ImGui::SameLine( );

        // Value
        {
            ImGui::BeginChild( "VView", ImVec2( ValueWindowWidth, 0 ), true, ImGuiWindowFlags_None );

            if ( ThisValueSaves != nullptr )
            {
                ToImGuiPointerSwitch( "Value", ThisValueSaves );
            }

            ImGui::EndChild( );
        }

        ImGui::EndChild( );
        ImGui::PopStyleVar( );

        ImGui::PopID( );
    }
}

template <template <typename, typename> class Container,
          typename Ty,
          typename Allocator = std::allocator<Ty>>
void
ToImGuiWidget( const char* Name, Container<Ty, Allocator>* Value )
{
    static std::string NameStrWithIndex;
    std::string        NameStr = NameStrWithIndex = Name;

    const int Digit = log10( Value->size( ) ) + 1;
    NameStrWithIndex.reserve( NameStr.size( ) + 2 + Digit );

    char* IndexPtrStart = NameStrWithIndex.data( ) + NameStr.size( );


    if ( ImGui::CollapsingHeader( Name, ImGuiTreeNodeFlags_OpenOnArrow ) )
    {
        for ( size_t Index = 0; Index < Value->size( ); Index++ )
        {
            snprintf( IndexPtrStart, NameStrWithIndex.capacity( ) + 1, "[%zu]", Index );
            ToImGuiPointerSwitch( NameStrWithIndex.c_str( ), Value->at( Index ) );
        }
    }
}

/*
 *
 * Is a double pointer without type impl
 *
 * */
template <typename Ty>
inline typename std::enable_if_t<std::is_pointer_v<std::remove_pointer_t<Ty>>, void>
ToImGuiPointerSwitch( const char* Name, Ty Value )
{
    if constexpr ( requires( std::remove_pointer_t<Ty> ValueRP ) { ToImGuiWidget( Name, ValueRP ); } )
    {
        ToImGuiWidget( Name, *Value );
    } else
    {
        ImGui::BeginDisabled( );
        ImGui::InputScalar( Name, ImGuiDataType_U64, (void*) Value, nullptr, nullptr, "%p", ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal );
        ImGui::EndDisabled( );
    }
}

/*
 *
 * Is a pointer
 *
 * */
template <typename Ty>
inline typename std::enable_if_t<!std::is_pointer_v<std::remove_pointer_t<Ty>> && std::is_pointer_v<Ty>, void>
ToImGuiPointerSwitch( const char* Name, Ty Value )
{
    if constexpr ( requires( Ty ValueReq ) { ToImGuiWidget( Name, ValueReq ); } )
    {
        ToImGuiWidget( Name, Value );
    } else
    {
        ImGui::InputScalar( Name, ImGuiDataType_U64, (void*) &Value, nullptr, nullptr, "%p", ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal );
    }
}

/*
 *
 * Is a value
 *
 * */
template <typename Ty>
inline typename std::enable_if_t<!std::is_pointer_v<Ty>, void>
ToImGuiPointerSwitch( const char* Name, Ty& Value )
{
    ToImGuiWidget( Name, &Value );
}