//
// Created by EMCJava on 3/18/2024.
//

#pragma once

#include <exception>
#include <cstdint>
#include <iomanip>
#include <span>

#include "DataBase/User.hpp"

// FIXME: A temporary workaround for a bug in CLion (Windows)
#define ASIO_HAS_CO_AWAIT 1
#include <asio.hpp>

namespace SanguisNet
{
struct MessageHeader {
    enum : uint32_t { ID_NONE,
                      ID_RESULT,
                      ID_GET,
                      ID_PUT,
                      ID_LOGIN,
                      ID_INFO,
                      ID_RECAST,
                      ID_FRIEND_LIST,
                      ID_LOBBY_CONTROL,
                      ID_LOBBY_LIST,
                      ID_GAME_PLAYER_LIST,
                      ID_GAME_GUN_FIRE,
                      ID_GAME_PLAYER_STAT,
                      ID_GAME_PLAYER_LAY_DOWN,
                      ID_GAME_PLAYER_SELF_LAY_DOWN,
                      ID_GAME_PLAYER_RECEIVE_DAMAGE,
                      ID_GAME_UPDATE_SELF_COORDINATES,
                      ID_GAME_UPDATE_PLAYER_COORDINATES,
    };
    uint32_t id     = 0;
    uint32_t length = 0;
};

static constexpr uint32_t MessageHeaderLength      = sizeof( MessageHeader );
static constexpr uint32_t MessageDataLength        = MessageHeaderLength * 8;
static constexpr uint32_t MaxNamesPreMessage       = SanguisNet::MessageDataLength / ( User::MaxNameLength + 1 + 1 /* name + \n + buffer */ );
static constexpr uint32_t MaxParticipantPerSection = MaxNamesPreMessage;

struct Message {
    MessageHeader header;
    uint8_t       data[ MessageDataLength ];

    [[nodiscard]] std::string_view ToString( ) const noexcept
    {
        return std::string_view { (std::string_view::pointer) data, header.length };
    }

    [[nodiscard]] bool StartWith( std::string_view Str ) const noexcept
    {
        return ToString( ).starts_with( Str );
    }

    [[nodiscard]] bool EndWith( std::string_view Str ) const noexcept
    {
        return ToString( ).ends_with( Str );
    }

    [[nodiscard]] bool operator==( std::string_view Str ) const noexcept
    {
        return ToString( ) == Str;
    }

    template <typename Ty, size_t Size>
    [[nodiscard]] bool operator==( const Ty ( &StrArr )[ Size ] ) const noexcept
    {
        return Size == header.length && std::equal( StrArr, StrArr + Size, data );
    }

    auto GetMinimalAsioBuffer( ) const
    {
        return asio::buffer( this, MessageHeaderLength + header.length );
    }

    template <typename Ty>
    Ty ToStruct( )
    {
        assert( header.length == sizeof( Ty ) );
        return std::bit_cast<Ty>( *reinterpret_cast<std::array<unsigned char, sizeof( Ty )>*>( data ) );
    }

    template <typename Ty>
    static Message FromStruct( Ty&& Data, int ID = MessageHeader::ID_NONE )
    {
        static_assert( sizeof( Data ) <= MessageDataLength, "Data too big" );

        Message msg;
        msg.header.id = ID;
        memcpy( msg.data, reinterpret_cast<const void*>( &Data ), msg.header.length = sizeof( Data ) );
        return msg;
    }

    static Message FromString( std::string_view Str, int ID = MessageHeader::ID_NONE )
    {
        Message msg;
        msg.header.id     = ID;
        msg.header.length = Str.size( );
        memcpy( msg.data, Str.data( ), Str.size( ) );
        return msg;
    }

    template <typename Ty, size_t Size>
    static Message FromString( const Ty ( &StrArr )[ Size ], int ID = MessageHeader::ID_NONE )
    {
        return FromString( std::string_view( StrArr, Size ), ID );
    }
};

inline std::ostream&
operator<<( std::ostream& os, const MessageHeader& header )
{
    return os << "[" << header.id << "," << header.length << "]";
}
inline std::ostream&
operator<<( std::ostream& os, const Message& message )
{
    os << std::hex;
    for ( uint8_t c : std::string_view( (char*) &message, sizeof( message ) ) )
        os << std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( c );
    return os << std::dec << std::setw( 0 );
}
namespace Game
{
    template <uint32_t MessageID>
    struct Formatter {
    };

    template <>
    struct Formatter<MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES> {
        struct PlayerIndexData {
            uint8_t            PlayerIndex { };
            std::span<uint8_t> Data { };
        };

        // Encode
        auto&
        operator( )( Message& Msg, int PlayerID ) const&
        {
            assert( Msg.header.length + 1 <= MessageDataLength );
            Msg.data[ Msg.header.length++ ] = static_cast<uint8_t>( PlayerID );
            return Msg;
        }

        // Decode
        PlayerIndexData operator( )( Message& Msg ) const&
        {
            --Msg.header.length;
            return PlayerIndexData {
                .PlayerIndex = Msg.data[ Msg.header.length ],
                .Data        = std::span<uint8_t>( Msg.data, Msg.header.length ) };
        }
    };

    template <>
    struct Formatter<MessageHeader::ID_GAME_GUN_FIRE> : Formatter<MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES> {
    };
    template <>
    struct Formatter<MessageHeader::ID_GAME_PLAYER_STAT> : Formatter<MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES> {
    };
    template <>
    struct Formatter<MessageHeader::ID_GAME_PLAYER_RECEIVE_DAMAGE> : Formatter<MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES> {
    };

    template <uint32_t MessageID>
    struct Encoder {
        decltype( auto ) operator( )( auto&&... Args ) const
        {
            return Formatter<MessageID> { }( std::forward<decltype( Args )>( Args )... );
        }
    };
    template <uint32_t MessageID>
    using Decoder = Encoder<MessageID>;
}   // namespace Game

}   // namespace SanguisNet