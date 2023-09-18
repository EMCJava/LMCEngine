#include "BaseBoard.h"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

void
SaBaseBoard::GetEffect( SaEffect& Result )
{
    std::vector<SaEffect> EffectCache;
    EffectCache.resize( RunOrder.size( ) );

    for ( size_t i = 0; i < RunOrder.size( ); ++i )
    {
        SaEffect FirstEffect;
        if ( RunOrder[ i ].FirstIndex >= SecondaryControlNodeOffset )
        {
            REQUIRED( RunOrder[ i ].FirstIndex >> MaxMosaickedControlNode < i, spdlog::critical( "Impossible EffectCache index" ); throw std::runtime_error( "Impossible EffectCache index" ); );
            FirstEffect = EffectCache[ RunOrder[ i ].FirstIndex >> MaxMosaickedControlNode ];
        } else
        {
            FirstEffect = MosaickedControlNode[ RunOrder[ i ].FirstIndex ]->GetEffect( );
        }

        SaEffect SecondEffect;
        if ( RunOrder[ i ].FirstIndex >= SecondaryControlNodeOffset )
        {
            REQUIRED( RunOrder[ i ].SecondIndex >> MaxMosaickedControlNode < i, spdlog::critical( "Impossible EffectCache index" ); throw std::runtime_error( "Impossible EffectCache index" ); );
            SecondEffect = EffectCache[ RunOrder[ i ].SecondIndex >> MaxMosaickedControlNode ];
        } else
        {
            SecondEffect = MosaickedControlNode[ RunOrder[ i ].SecondIndex ]->GetEffect( );
        }

        Result = EffectCache[ i ] = Combine( RunOrder[ i ].CombineMethod, FirstEffect, SecondEffect );
    }
}

SaBaseBoard::SaBaseBoard( )
{
}

void
SaBaseBoard::AddDemoData( )
{

    MosaickedControlNode.reserve( 4 );
    MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );
    MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );
    MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );
    MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );

    REQUIRED( MosaickedControlNode.size( ) <= MaxMosaickedControlNode )

    constexpr SaCombineMethod CombMix = SaCombineMethod::CombineMethodMix;
    RunOrder                          = {
        {                         0,                              1, CombMix},
        {                         0,                              1, CombMix},
        {                         0,                              1, CombMix},
        {                         0,                              1, CombMix},
        {                         0,                              1, CombMix},
        {SecondaryControlNodeOffset, SecondaryControlNodeOffset + 1, CombMix},
    };
}
