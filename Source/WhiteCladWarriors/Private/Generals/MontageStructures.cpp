
#include "Generals/Structs/MontageStructures.h"

void FMontageEventInfo::Clear()
{
	MontageToPlay = nullptr;
	PlayRate = 1.0f;
	StartingPosition = 0.0f;
	BlendInTime = 0.2f;
	BlendOutTime = 0.2f;
	bIsStarted = false;
}

bool FMontageEventInfo::Play(UAnimInstance* Anim, bool bIsStopOtherMontage)
{
	if (!IsValid(Anim)) return false;
	if (IsValid(MontageToPlay))
	{
		Anim->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition, false);
		bIsStarted = true;
	}
	return bIsStarted;
}

void FMontageEventInfo::Stop(UAnimInstance* Anim)
{
	bIsStarted = false;
	if (!IsValid(Anim)) return;
	if (IsValid(MontageToPlay))
	{
		Anim->Montage_Stop(BlendOutTime, MontageToPlay);
	}
}