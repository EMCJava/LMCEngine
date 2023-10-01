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

public:
    SaBaseBoard( );

    virtual void
    GetEffect( SaEffect& Result ) override;

    void
    SetSlot( size_t Index, std::shared_ptr<SaControlNode> ControlNode );

    void
    Serialize( const std::string& JsonStr );

protected:
    std::vector<SaRunOrder>                     m_RunOrder;
    std::vector<std::shared_ptr<SaControlNode>> m_MosaickedControlNode;
    std::map<std::string, size_t>               m_MosaickedControlNodeNameMap;
};
