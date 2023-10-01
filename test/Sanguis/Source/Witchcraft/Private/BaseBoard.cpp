#include "BaseBoard.h"
#include "ControlNodeSimpleEffect.hpp"
#include "ControlNodePlaceholder.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <yaml-cpp/yaml.h>

#include <ranges>
#include <deque>
#include <list>
#include <utility>

void
SaBaseBoard::GetEffect( SaEffect& Result )
{
    std::vector<SaEffect> EffectCache;
    EffectCache.resize( m_RunOrder.size( ) );

    for ( size_t i = 0; i < m_RunOrder.size( ); ++i )
    {
        if ( m_RunOrder[ i ].CombineMethod == SaCombineMethod::CombineMethodNone )
        {
            TEST( m_RunOrder[ i ].FirstIndex == m_RunOrder[ i ].SecondIndex && m_RunOrder[ i ].FirstIndex < m_MosaickedControlNode.size( ) );
            Result = EffectCache[ i ] = m_MosaickedControlNode[ m_RunOrder[ i ].FirstIndex ]->GetEffect( );
        } else
        {
            SaEffect FirstEffect  = EffectCache[ m_RunOrder[ i ].FirstIndex ];
            SaEffect SecondEffect = EffectCache[ m_RunOrder[ i ].SecondIndex ];

            Result = EffectCache[ i ] = Combine( m_RunOrder[ i ].CombineMethod, FirstEffect, SecondEffect );
        }
    }
}

SaBaseBoard::SaBaseBoard( )
{
}

struct ParseCompose {

    // Set at runtime
    bool            base_slot = false;
    std::string     id;
    std::string     first_id;
    std::string     second_id;
    SaCombineMethod method;
};

namespace YAML
{
template <>
struct convert<SaCombineMethod> {
    static Node encode( const SaCombineMethod& rhs )
    {
        switch ( rhs )
        {
        case SaCombineMethod::CombineMethodNone: return Node( "none" );
        case SaCombineMethod::CombineMethodRight: return Node( "right" );
        case SaCombineMethod::CombineMethodLeft: return Node( "left" );
        case SaCombineMethod::CombineMethodMix: return Node( "mix" );
        }
    }

    static bool decode( const Node& node, SaCombineMethod& rhs )
    {
        const auto str = node.as<std::string>( );

        if ( str == "none" )
        {
            rhs = SaCombineMethod::CombineMethodNone;
            return true;
        } else if ( str == "right" )
        {
            rhs = SaCombineMethod::CombineMethodRight;
            return true;
        } else if ( str == "left" )
        {
            rhs = SaCombineMethod::CombineMethodLeft;
            return true;
        } else if ( str == "mix" )
        {
            rhs = SaCombineMethod::CombineMethodMix;
            return true;
        }

        return false;
    }
};

template <>
struct convert<ParseCompose> {
    static Node encode( const ParseCompose& rhs )
    {
        Node node;
        node[ "id" ]        = rhs.id;
        node[ "first_id" ]  = rhs.first_id;
        node[ "second_id" ] = rhs.second_id;
        node[ "method" ]    = rhs.method;
        return node;
    }

    static bool decode( const Node& node, ParseCompose& rhs )
    {
        if ( !node.IsMap( ) || node.size( ) != 4 )
        {
            return false;
        }

        rhs.id        = node[ "id" ].as<std::string>( );
        rhs.first_id  = node[ "first_id" ].as<std::string>( );
        rhs.second_id = node[ "second_id" ].as<std::string>( );
        rhs.method    = node[ "method" ].as<SaCombineMethod>( );
        return true;
    }
};
}   // namespace YAML

void
SaBaseBoard::Serialize( const std::string& JsonStr )
{
    auto BoardTemplate = YAML::Load( JsonStr );

    /*
     *
     * Read configuration
     *
     * */
    m_MosaickedControlNode.clear( );
    auto SlotIDs = BoardTemplate[ "slots" ].as<std::vector<std::string>>( );
    m_MosaickedControlNode.resize( SlotIDs.size( ) );

    auto EffectCompose    = BoardTemplate[ "compose" ].as<std::map<std::string, ParseCompose>>( );
    auto EffectiveCompose = BoardTemplate[ "effective_compose" ].as<std::vector<std::string>>( );

    /*
     *
     * Push necessary effects
     *
     * */
    std::deque<std::string> LeftCompose;
    for ( auto& it : std::ranges::reverse_view( EffectiveCompose ) )
        LeftCompose.push_back( it );

    /*
     *
     * Record base slot (not composed)
     *
     * */
    for ( const auto& slot : SlotIDs )
        EffectCompose[ slot ] = { .base_slot = true, .id = slot };

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

        if ( Compose.base_slot )
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
    m_RunOrder.clear( );
    std::map<std::string, size_t> m_RunOrderIndex;

    // Make sure all base slot runs first
    auto SlotExtraction = ExecuteOrderTmp
        | std::views::filter( [ &SlotIDs ]( const auto& id ) { return std::find( SlotIDs.begin( ), SlotIDs.end( ), id ) != SlotIDs.end( ); } )
        | std::views::reverse;

    for ( const auto& Slot : SlotExtraction )
    {
        const size_t Index      = std::distance( SlotIDs.begin( ), std::find( SlotIDs.begin( ), SlotIDs.end( ), Slot ) );
        m_RunOrderIndex[ Slot ] = m_RunOrder.size( );
        m_RunOrder.push_back( SaRunOrder { Index, Index, SaCombineMethod::CombineMethodNone } );
    }

    // Finish by combining all the compose effects
    auto ComposeExtraction = ExecuteOrderTmp
        | std::views::filter( [ &SlotIDs ]( const auto& id ) { return std::find( SlotIDs.begin( ), SlotIDs.end( ), id ) == SlotIDs.end( ); } )
        | std::views::reverse;

    for ( const auto& ComposeID : ComposeExtraction )
    {
        // All sub-Compose should be in m_RunOrderIndex
        const auto& Compose          = EffectCompose[ ComposeID ];
        m_RunOrderIndex[ ComposeID ] = m_RunOrder.size( );
        m_RunOrder.push_back( SaRunOrder { m_RunOrderIndex[ Compose.first_id ], m_RunOrderIndex[ Compose.second_id ], Compose.method } );
    }

    /*
     *
     * Fill all slot with empty
     *
     * */
    for ( size_t i = 0; i < m_MosaickedControlNode.size( ); ++i )
    {
        SetSlot( i, std::make_shared<SaControlNodePlaceholder>( ) );
        m_MosaickedControlNodeNameMap[ SlotIDs[ i ] ] = i;
    }
}

bool
SaBaseBoard::SetSlot( size_t Index, std::shared_ptr<SaControlNode> ControlNode )
{
    if ( Index >= m_MosaickedControlNode.size( ) ) return false;

    m_MosaickedControlNode[ Index ] = std::move( ControlNode );
    return true;
}

bool
SaBaseBoard::SetSlot( const std::string& ID, std::shared_ptr<SaControlNode> ControlNode )
{
    const auto It = m_MosaickedControlNodeNameMap.find( ID );

    if ( It != m_MosaickedControlNodeNameMap.end( ) )
    {
        return SetSlot( It->second, std::move( ControlNode ) );
    }

    return false;
}
