#include "Witchcraft/BaseBoard.h"

void SaBaseBoard::GetEffect(SaEffect& Result)
{
	TArray<SaEffect> EffectCache;
	EffectCache.SetNum(RunOrder.Num());

	for (int32 i = 0; i < RunOrder.Num(); ++i)
	{
		SaEffect FirstEffect;
		if (RunOrder[i].FirstIndex >= SecondaryControlNodeOffset)
		{
			ensureAlwaysMsgf(RunOrder[i].FirstIndex >> MaxMosaickedControlNode < i, TEXT("Inpossibe EffectCache index"));
			FirstEffect = EffectCache[RunOrder[i].FirstIndex >> MaxMosaickedControlNode];
		}
		else
		{
			FirstEffect = MosaickedControlNode[RunOrder[i].FirstIndex]->GetEffect();
		}

		SaEffect SecondEffect;
		if (RunOrder[i].FirstIndex >= SecondaryControlNodeOffset)
		{
			ensureAlwaysMsgf(RunOrder[i].SecondIndex >> MaxMosaickedControlNode < i, TEXT("Inpossibe EffectCache index"));
			SecondEffect = EffectCache[RunOrder[i].SecondIndex >> MaxMosaickedControlNode];
		}
		else
		{
			SecondEffect = MosaickedControlNode[RunOrder[i].SecondIndex]->GetEffect();
		}

		Result = EffectCache[i] = Combine(RunOrder[i].CombineMethod, FirstEffect, SecondEffect);
	}
}

SaBaseBoard::SaBaseBoard()
{
}