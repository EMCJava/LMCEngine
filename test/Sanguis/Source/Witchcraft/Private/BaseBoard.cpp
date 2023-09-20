#include "BaseBoard.h"
#include "ControlNodeSimpleEffect.hpp"
#include "ControlNodePlaceholder.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <nlohmann/json.hpp>

#include <ranges>
#include <deque>

void
SaBaseBoard::GetEffect( SaEffect& Result )
{
    std::vector<SaEffect> EffectCache;
    EffectCache.resize( RunOrder.size( ) );

    for ( size_t i = 0; i < RunOrder.size( ); ++i )
    {
        if ( RunOrder[ i ].CombineMethod == SaCombineMethod::CombineMethodNone )
        {
            TEST( RunOrder[ i ].FirstIndex == RunOrder[ i ].SecondIndex && RunOrder[ i ].FirstIndex < MosaickedControlNode.size( ) );
            Result = EffectCache[ i ] = MosaickedControlNode[ RunOrder[ i ].FirstIndex ]->GetEffect( );
        } else
        {
            SaEffect FirstEffect  = EffectCache[ RunOrder[ i ].FirstIndex ];
            SaEffect SecondEffect = EffectCache[ RunOrder[ i ].SecondIndex ];

            Result = EffectCache[ i ] = Combine( RunOrder[ i ].CombineMethod, FirstEffect, SecondEffect );
        }
    }
}

SaBaseBoard::SaBaseBoard( )
{
}

struct ParseCompose {

    bool            fromSlot = false;
    std::string     id;
    std::string     first_id;
    std::string     second_id;
    SaCombineMethod method;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ParseCompose, id, first_id, second_id, method )
NLOHMANN_JSON_SERIALIZE_ENUM( SaCombineMethod, {
                                                   { SaCombineMethod::CombineMethodNone,  "none"},
                                                   {SaCombineMethod::CombineMethodRight, "right"},
                                                   { SaCombineMethod::CombineMethodLeft,  "left"},
                                                   {  SaCombineMethod::CombineMethodMix,   "mix"},
} )

void
SaBaseBoard::Serialize( const std::string& JsonStr )
{
    auto BoardTemplate = nlohmann::json::parse( JsonStr );

    /*
     *
     * Read configuration
     *
     * */
    MosaickedControlNode.clear( );
    std::vector<std::string> SlotIDs = BoardTemplate[ "slots" ];
    MosaickedControlNode.resize( SlotIDs.size( ) );

    std::map<std::string, ParseCompose> EffectCompose    = BoardTemplate[ "compose" ];
    std::vector<std::string>            EffectiveCompose = BoardTemplate[ "effective_compose" ];

    /*
     *
     * Push necessary effects
     *
     * */
    std::deque<std::string> LeftCompose;
    for ( auto it = EffectiveCompose.rbegin( ); it != EffectiveCompose.rend( ); ++it )
        LeftCompose.push_back( *it );

    /*
     *
     * Record base slot (not composed)
     *
     * */
    for ( const auto& slot : SlotIDs )
        EffectCompose[ slot ] = { .fromSlot = true, .id = slot };

    /*
     *
     * Arrange all effects, make sure all sub-effects are calculated first
     *
     * */
    std::list<std::string> ExecuteOrderTmp, ExecuteOrder;
    while ( !LeftCompose.empty( ) )
    {
        auto&         ID      = LeftCompose.front( );
        ParseCompose& Compose = EffectCompose.at( ID );

        if ( Compose.fromSlot )
        {
            if ( std::find( ExecuteOrderTmp.begin( ), ExecuteOrderTmp.end( ), Compose.id ) == ExecuteOrderTmp.end( ) )
                ExecuteOrderTmp.push_back( Compose.id );
            LeftCompose.pop_front( );
            continue;
        }

        //        if ( std::find( ExecuteOrderTmp.begin( ), ExecuteOrderTmp.end( ), Compose.id ) != ExecuteOrderTmp.end( ) )
        //        {
        //            // Already added
        //            continue;
        //        }

        const auto FirstIt  = std::find( ExecuteOrderTmp.begin( ), ExecuteOrderTmp.end( ), Compose.first_id );
        const auto SecondIt = std::find( ExecuteOrderTmp.begin( ), ExecuteOrderTmp.end( ), Compose.second_id );

        if ( FirstIt == ExecuteOrderTmp.end( ) )
        {
            LeftCompose.push_front( Compose.first_id );
        }

        if ( SecondIt == ExecuteOrderTmp.end( ) )
        {
            LeftCompose.push_front( Compose.second_id );
        }

        if ( FirstIt != ExecuteOrderTmp.end( ) && SecondIt != ExecuteOrderTmp.end( ) )
        {
            const auto EarliestIt = ( std::distance( ExecuteOrderTmp.begin( ), FirstIt ) < std::distance( ExecuteOrderTmp.begin( ), SecondIt ) ) ? FirstIt : SecondIt;
            ExecuteOrderTmp.insert( EarliestIt, Compose.id );
            LeftCompose.pop_front( );
        }
    }

    /*
     *
     * Generate final run order in index
     *
     * */
    RunOrder.clear( );
    std::map<std::string, size_t> RunOrderIndex;

    // Make sure all base slot runs first
    auto SlotExtraction = ExecuteOrderTmp
        | std::views::filter( [ &SlotIDs ]( const auto& id ) { return std::find( SlotIDs.begin( ), SlotIDs.end( ), id ) != SlotIDs.end( ); } )
        | std::views::reverse;

    for ( const auto& Slot : SlotExtraction )
    {
        const size_t Index    = std::distance( SlotIDs.begin( ), std::find( SlotIDs.begin( ), SlotIDs.end( ), Slot ) );
        RunOrderIndex[ Slot ] = RunOrder.size( );
        RunOrder.push_back( SaRunOrder { Index, Index, SaCombineMethod::CombineMethodNone } );
    }

    // Finish by combining all the compose effects
    auto ComposeExtraction = ExecuteOrderTmp
        | std::views::filter( [ &SlotIDs ]( const auto& id ) { return std::find( SlotIDs.begin( ), SlotIDs.end( ), id ) == SlotIDs.end( ); } )
        | std::views::reverse;

    for ( const auto& ComposeID : ComposeExtraction )
    {
        // All sub-Compose should be in RunOrderIndex
        const auto& Compose        = EffectCompose[ ComposeID ];
        RunOrderIndex[ ComposeID ] = RunOrder.size( );
        RunOrder.push_back( SaRunOrder { RunOrderIndex[ Compose.first_id ], RunOrderIndex[ Compose.second_id ], Compose.method } );
    }

    /*
     *
     * Fill all slot with empty
     *
     * */
    for ( size_t i = 0; i < MosaickedControlNode.size( ); ++i )
    {
        SetSlot( i, std::make_shared<SaControlNodePlaceholder>( ) );
    }
}

void
SaBaseBoard::SetSlot( size_t Index, std::shared_ptr<SaControlNode> ControlNode )
{
    MosaickedControlNode[ Index ] = std::move( ControlNode );
}
