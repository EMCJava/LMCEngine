//
// Created by EMCJava on 3/18/2024.
//

#pragma once

#include <exception>
#include <cstdint>
#include <iomanip>

// FIXME: A temporary workaround for a bug in CLion (Windows)
#define ASIO_HAS_CO_AWAIT 1
#include <asio.hpp>

namespace SanguisNet
{
struct MessageHeader {
    enum { ID_NONE,
           ID_RESULT,
           ID_LOGIN };
    uint32_t id     = 0;
    uint32_t length = 0;
};

static constexpr uint32_t MessageHeaderLength = sizeof( MessageHeader );
static constexpr uint32_t MessageDataLength   = MessageHeaderLength * 4;

struct Message {
    MessageHeader header;
    uint8_t       data[ MessageDataLength ];

    auto GetMinimalAsioBuffer( ) const
    {
        return asio::buffer( this, MessageHeaderLength + header.length );
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
}   // namespace SanguisNet