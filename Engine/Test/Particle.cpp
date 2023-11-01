//
// Created by samsa on 10/27/2023.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <Engine/Core/Graphic/Sprites/Particle/ParticleAttributesRandomizer.hpp>

#include <algorithm>

#include <spdlog/spdlog.h>

struct ParticleTimeGreater {
    _NODISCARD bool operator( )( Particle& Left, Particle& Right ) const
        noexcept( noexcept( Left.GetLifeTime( ) > Right.GetLifeTime( ) ) )
    {
        return Left.GetLifeTime( ) > Right.GetLifeTime( );
    }
};

TEST_CASE( "Particle Min heap", "[Particle]" )
{
    std::vector<Particle> Particles;
    Particles.resize( 200000 );
    size_t ActualSize = 0;

    ParticleAttributesRandomizer PAR;
    PAR.SetLinearScale( { 0.01, 0.01, 0.01 }, { 0.1, 0.1, 0.1 } );
    PAR.SetVelocity( { -20, -20, 0 }, { 20, 20, 0 } );
    PAR.SetAngularVelocity( -31.415F, 31.415F );
    PAR.SetStartLinearColor( glm::vec4( 1, 0, 0, 0.9 ), glm::vec4( 1, 0.2, 0, 1 ) );
    PAR.SetEndLinearColor( glm::vec4( 1, 0.6, 0.4, 0 ), glm::vec4( 1, 0.8, 0.6, 0 ) );
    PAR.SetLifetime( 1, 50 );

    const auto PushParticles = [ &Particles, &ActualSize ]( ParticleAttributesRandomizer& PAR ) {
        if ( ActualSize >= Particles.size( ) ) return;
        PAR.Apply( Particles[ ActualSize ] );
        std::ranges::push_heap( Particles.begin( ), Particles.begin( ) + ActualSize, ParticleTimeGreater { } );
        ActualSize++;
    };

    const auto Update = [ &Particles, &ActualSize ]( FloatTy DeltaTime ) {
        if ( ActualSize == 0 ) return;

        for ( size_t i = 0; i < ActualSize; ++i )
        {
            Particles[ i ].Update( DeltaTime );
        }

        while ( ActualSize >= 0 && !Particles.front( ).IsAlive( ) )
        {
            std::ranges::pop_heap( Particles.begin( ), Particles.begin( ) + ActualSize, ParticleTimeGreater { } );
            ActualSize--;
        }
    };

    BENCHMARK( "Particle Push" )
    {
        ActualSize = 0;
        for ( int i = 0; i < 200000; ++i )
        {
            PushParticles( PAR );
        }
    };

    BENCHMARK( "Particle Expire" )
    {
        ActualSize = 0;
        for ( int i = 0; i < 200000; ++i )
        {
            PushParticles( PAR );
        }

        for ( int i = 0; i < 1000; ++i )
        {
            Update( 0.03 );
        }

        spdlog::info( "Actual Size: {}", ActualSize );
    };
}