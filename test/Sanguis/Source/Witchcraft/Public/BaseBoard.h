#pragma once

#include "ControlNode.h"
#include "EffectMixer.h"

#include <vector>
#include <memory>
#include <cmath>

class SaBaseBoard : public SaControlNode
{
public:
    struct SaRunOrder {
        size_t          FirstIndex, SecondIndex;
        SaCombineMethod CombineMethod;
    };

    SaBaseBoard( const SaBaseBoard& Other )                = delete;
    SaBaseBoard( SaBaseBoard&& Other ) noexcept            = delete;
    SaBaseBoard& operator=( const SaBaseBoard& Other )     = delete;
    SaBaseBoard& operator=( SaBaseBoard&& Other ) noexcept = delete;

protected:
    std::vector<SaRunOrder>                     RunOrder;
    std::vector<std::shared_ptr<SaControlNode>> MosaickedControlNode;

public:
    SaBaseBoard( );

    virtual void
    GetEffect( SaEffect& Result ) override;

    void
    SetSlot( size_t Index, std::shared_ptr<SaControlNode> ControlNode );

    void
    Serialize( const std::string& JsonStr );
};
