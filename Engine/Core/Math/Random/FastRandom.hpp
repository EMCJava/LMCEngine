//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <array>
#include <cstdint>
#include <ostream>

class FastRandom
{
private:
    union NoiseType
    {
        uint64_t                SeedArray[ 2 ];
        std::array<uint64_t, 2> Seed;
        std::array<int32_t, 4>  SubSeed;
    };

    // Update after every seed change
    uint64_t SeekValue;

public:
    constexpr FastRandom( const NoiseType& Seed = { } );

    constexpr FastRandom( const FastRandom& other );

    constexpr FastRandom&
    operator=( const FastRandom& other );

    [[nodiscard]] constexpr const NoiseType&
    GetSeed( ) const;

    constexpr std::array<uint64_t, 2>
    CopySeed( ) const;

    constexpr void
    SetSeed( const std::array<uint64_t, 2>& Seed );

    constexpr void
    SetSeed( const std::array<int32_t, 4>& Seed );

    constexpr uint64_t
    SeekUint64( ) const noexcept;

    /*
     *
     * Get cached seek value, prefer using this
     *
     * */
    constexpr uint64_t
    GetSeekValue( ) const noexcept { return SeekValue; };

    /*
     *
     * This function will modify the Seed
     *
     * */
    constexpr uint64_t
    NextUint64( );

    constexpr FastRandom&
    Jump( ) noexcept;

    static constexpr FastRandom
    FromUint64( uint64_t Seed );

    static FastRandom
    FromRand( );

    bool constexpr
    operator==( const FastRandom& oth ) const;

    inline friend std::ostream&
    operator<<( std::ostream& os, const FastRandom& n );

private:
    NoiseType m_Seed { };

    [[nodiscard]] static constexpr int
    GetIntSeed( const NoiseType& Seed );

    static constexpr uint64_t
    ROTL( uint64_t x, int k );
};

#include "FastRandom.inl"