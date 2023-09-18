// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ControlNode.h"
#include "EffectMixer.h"

#include <vector>
#include <memory>
#include <cmath>

constexpr uint32_t MaxMosaickedControlNode = 4;

// For any index bigger than SecondaryControlNodeOffset
// it refer to result of previous mixed effect, namely index of MosaickedControlNode
constexpr int
countBits( uint32_t num )
{
    int count = 0;
    while ( num != 0 )
    {
        num >>= 1;
        count++;
    }
    return count;
}
constexpr uint32_t SecondaryControlNodeOffset = (uint32_t) 1 << ( countBits( MaxMosaickedControlNode ) + 1 );

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

    void AddDemoData( );
};
