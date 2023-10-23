//
// Created by loyus on 8/12/2023.
//

#pragma once

#include <ImGui/Ext/ImGuiGroup.hpp>
#include <Engine/Core/Math/Random/FastRandom.hpp>

#include <map>
#include <memory>
#include <type_traits>
#include <variant>

#include <ImGui/ImGui.hpp>

#include <glm/glm.hpp>

/*
 *
 * Is invokable switches
 *
 * */
template <typename Ty, typename MemPtr>
inline typename std::enable_if_t<std::is_member_function_pointer_v<std::remove_pointer_t<MemPtr>>, void>
ToImGuInvokableSwitch( const char* Name, Ty Value, MemPtr MemberPtr );
template <typename Ty, typename MemPtr>
inline typename std::enable_if_t<std::is_member_object_pointer_v<std::remove_pointer_t<MemPtr>>, void>
ToImGuInvokableSwitch( const char* Name, Ty Value, MemPtr MemberPtr );

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
ToImGuiWidget( const char* Name, glm::vec3* Value )
{
    static_assert( sizeof( glm::vec3 ) == sizeof( float ) * 3 );
    static_assert( std::is_same_v<FloatTy, float> );
    ImGui::DragFloat3( Name, (FloatTy*) Value );
}

inline void
ToImGuiWidget( const char* Name, glm::vec4* Value )
{
    static_assert( sizeof( glm::vec4 ) == sizeof( float ) * 4 );
    static_assert( std::is_same_v<FloatTy, float> );
    ImGui::DragFloat4( Name, (FloatTy*) Value );
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
ToImGuiWidget( const char* Name, uint8_t* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_U8, Value );
}

inline void
ToImGuiWidget( const char* Name, float* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_Float, Value );
}

namespace
{

static int
InputTextCallback( ImGuiInputTextCallbackData* data )
{
    if ( data->EventFlag == ImGuiInputTextFlags_CallbackResize )
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        auto* str = (std::string*) data->UserData;
        IM_ASSERT( data->Buf == str->c_str( ) );
        str->resize( data->BufTextLen );
        data->Buf = (char*) str->c_str( );
    }

    return 0;
}
}   // namespace

inline void
ToImGuiWidget( const char* Name, std::string* Value )
{
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;
    ImGui::InputText( Name, (char*) Value->c_str( ), Value->capacity( ) + 1, flags, ::InputTextCallback, Value );
}

inline void
ToImGuiWidget( const char* Name, FastRandom* Value )
{
    const auto& Seed = Value->GetSeed( );
    ImGui::InputScalarN( Name, ImGuiDataType_S64, (void*) Seed.Seed.data( ), 2 );
}

/*
 *
 * std containers
 *
 * */
template <typename Ty1, typename Ty2>
inline void
ToImGuiWidget( const char* Name, std::pair<Ty1, Ty2>* Value )
{
    std::string NameStr = Name;
    ToImGuiPointerSwitch( ( NameStr + ".first" ).c_str( ), &Value->first );
    ImGui::SameLine( );
    ToImGuiPointerSwitch( ( NameStr + ".second" ).c_str( ), &Value->second );
}

template <typename Ty>
inline void
ToImGuiWidget( const char* Name, std::weak_ptr<Ty>* Value )
{
    if ( Value->expired( ) )
    {
        ImGui::TextDisabled( "%s [ REF EXPIRED ]", Name );
    }

    ToImGuiPointerSwitch( Name, Value->lock( ).get( ) );
}

template <typename Ty>
inline void
ToImGuiWidget( const char* Name, std::shared_ptr<Ty>* Value )
{
    ToImGuiPointerSwitch( Name, Value->get( ) );
}

template <typename Ty>
inline void
ToImGuiWidget( const char* Name, std::unique_ptr<Ty>* Value )
{
    ToImGuiPointerSwitch( Name, Value->get( ) );
}

template <typename... Args>
inline void
ToImGuiWidget( const char* Name, std::variant<Args...>* Value )
{
    std::visit( [ Name ]( auto& Value ) {
        ToImGuiPointerSwitch( Name, &Value );
    },
                *Value );
}

/*
 *
 * Container such as map
 *
 * */
template <typename Ty>
concept MapLikeContainer = requires( Ty Map, typename Ty::key_type Key ) {
    typename Ty::key_type;
    typename Ty::mapped_type;
    {
        Map.at( Key )
    } -> std::same_as<typename Ty::mapped_type&>;   // Checked key access
    {
        Map.count( Key )
    } -> std::same_as<size_t>;   // Key existence check
    {
        *Map.begin( )
    } -> std::same_as<std::pair<const typename Ty::key_type, typename Ty::mapped_type>&>;   // Begin iterator
};

template <template <typename, typename, typename, typename> class Container,
          typename KTy,
          typename VTy,
          typename Comparator = std::less<KTy>,
          typename Allocator  = std::allocator<std::pair<KTy, VTy>>>
    requires MapLikeContainer<Container<KTy, VTy, Comparator, Allocator>>
inline void
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

template <typename T>
concept VecLikeContainer = requires( T* Vec, const T* ConstVec ) {
    ConstVec->size( );
    {
        ( *ConstVec )[ (size_t) 0 ]
    } -> std::same_as<const typename T::value_type&>;
    {
        ConstVec->at( (size_t) 0 )
    } -> std::same_as<const typename T::value_type&>;
    {
        Vec->data( )
    } -> std::same_as<typename T::value_type*>;
};

template <VecLikeContainer Container>
inline void
ToImGuiWidget( const char* Name, Container* Value )
{
    static std::string NameStrWithIndex;
    std::string        NameStr = NameStrWithIndex = Name;

    if ( Value->size( ) != 0 )
    {
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
    } else
    {
        ImGui::TextDisabled( "%s [ CONTAINER EMPTY ]", Name );
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
        if ( Value == nullptr )
        {
            ImGui::TextDisabled( "%s [ NULL ]", Name );
        } else
        {
            ToImGuiWidget( Name, Value );
        }
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

/*
 *
 * Is a invokable member function
 *
 * */
template <typename Ty, typename MemPtr>
inline typename std::enable_if_t<std::is_member_function_pointer_v<std::remove_pointer_t<MemPtr>>, void>
ToImGuInvokableSwitch( const char* Name, Ty Value, MemPtr MemberPtr )
{
    if ( ImGui::Button( Name ) )
    {
        ( Value->*( MemberPtr ) )( );
    }
}

/*
 *
 * Is a member object
 *
 * */
template <typename Ty, typename MemPtr>
inline typename std::enable_if_t<std::is_member_object_pointer_v<std::remove_pointer_t<MemPtr>>, void>
ToImGuInvokableSwitch( const char* Name, Ty Value, MemPtr MemberPtr )
{
    ToImGuiPointerSwitch( Name, Value->*MemberPtr );
}
