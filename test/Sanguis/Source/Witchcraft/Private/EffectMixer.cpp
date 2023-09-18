#include "EffectMixer.h"
#include "ControlNode.h"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

// Use A to modify B
bool
CombineRight( const SaEffect& A, const SaEffect& B, SaEffect& Result )
{
    if ( !A.IsModifier )
        return false;

    Result.IsModifier = B.IsModifier;

    return true;
}

// Use B to modify A
inline bool
CombineLeft( const SaEffect& A, const SaEffect& B, SaEffect& Result )
{
    return CombineRight( B, A, Result );
}

bool
CombineMix( const SaEffect& A, const SaEffect& B, SaEffect& Result )
{
    return false;
}

SaEffect
Combine( const SaCombineMethod Method, const SaEffect& A, const SaEffect& B )
{
    SaEffect Result;
    Result.Iteration = A.Iteration + B.Iteration + 1;

#define CombineMethod( Method )                  \
    case SaCombineMethod::CombineMethod##Method: \
        Combine##Method( A, B, Result );         \
        break

    switch ( Method )
    {
        CombineMethod( Right );
        CombineMethod( Left );
        CombineMethod( Mix );

    case SaCombineMethod::CombineMethodNone:
    default:
        TEST( false );
    }

#undef CombineMethod

    return Result;
}
