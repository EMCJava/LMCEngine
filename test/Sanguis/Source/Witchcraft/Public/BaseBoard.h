// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ControlNode.h"
#include "EffectMixer.h"

#include <vector>
#include <memory>

constexpr uint32_t MaxMosaickedControlNode = 1;

// For any index bigger than SecondaryControlNodeOffset
// it refer to result of previous mixed effect, namely index of MosaickedControlNode
constexpr uint32_t SecondaryControlNodeOffset = 1 << MaxMosaickedControlNode;

class SaBaseBoard : public SaControlNode
{
public:
    struct SaRunOrder {
        // Please read comment of SecondaryControlNodeOffset
        int32_t         FirstIndex, SecondIndex;
        SaCombineMethod CombineMethod;
    };

    SaBaseBoard( const SaBaseBoard& Other )                = delete;
    SaBaseBoard( SaBaseBoard&& Other ) noexcept            = delete;
    SaBaseBoard& operator=( const SaBaseBoard& Other )     = delete;
    SaBaseBoard& operator=( SaBaseBoard&& Other ) noexcept = delete;

protected:
    std::vector<SaRunOrder> RunOrder;

    std::vector<std::unique_ptr<SaControlNode>> MosaickedControlNode;

public:
    SaBaseBoard( );

    virtual void GetEffect( SaEffect& Result ) override;

    void AddDemoData( )
    {

        MosaickedControlNode.reserve( 4 );
        MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );
        MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );
        MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );
        MosaickedControlNode.emplace_back( std::make_unique<SaBaseBoard>( ) );

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
};
