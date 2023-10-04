#pragma once

#include <cstdint>
#include <string>

enum Element : uint8_t {
    Fire   = 0,
    Water  = 1,
    Earth  = 2,
    Air    = 3,
    Aether = 4,
    NumOfElement
};

inline std::string
ToString( Element ElementType )
{
    switch ( ElementType )
    {
    case Element::Fire:
        return "Fire";
    case Element::Water:
        return "Water";
    case Earth:
        return "Earth";
    case Air:
        return "Air";
    case Aether:
        return "Aether";
    case NumOfElement:
    default:
        return "Nan";
    }
}