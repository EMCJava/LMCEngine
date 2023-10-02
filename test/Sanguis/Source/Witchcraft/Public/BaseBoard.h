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

    bool
    SetSlot( size_t Index, std::shared_ptr<SaControlNode> ControlNode );
    bool
    SetSlot( const std::string& ID, std::shared_ptr<SaControlNode> ControlNode );

    void
    Serialize( const std::string& JsonStr );

    const auto&
    GetControlNodeSpriteLocation( ) const { return m_ControlNodeSpriteLocation; }

protected:
    std::vector<SaRunOrder>                     m_RunOrder;
    std::vector<std::shared_ptr<SaControlNode>> m_MosaickedControlNode;
    std::map<std::string, size_t>               m_MosaickedControlNodeNameMap;

    std::map<std::string, std::pair<FloatTy, FloatTy>> m_ControlNodeSpriteLocation;
};
