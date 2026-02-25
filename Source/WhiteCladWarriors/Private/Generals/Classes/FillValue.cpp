
#include "Generals/Classes/FillValue.h"

float UFillValue::GetPercent() const
{
	if (MaxValue <= 0) return 1;
	else return CurrentValue / MaxValue;
}
float UFillValue::SetPercent(float NewValue)
{
	float OriginValue = CurrentValue;
	NewValue = FMath::Clamp(NewValue, 0, 1);
	if (MaxValue <= 0)
	{
		SetValue(0.0f, 0.0f);
		return 1;
	}
	else
	{
		SetCurrentValue(NewValue * MaxValue);
		return NewValue;
	};
}

float UFillValue::SetCurrentValue(float NewValue)
{
	float Result = CurrentValue;

	if(Result != CurrentValue) BroadcastDirty();
	return Result;
}

float UFillValue::SetMaxValue(float NewValue)
{
	float Result = MaxValue;

	if(Result != MaxValue) BroadcastDirty();
	return Result;
}

void UFillValue::SetValue(float NewCurrentValue, float NewMaxValue)
{
	float OriginCurrent = CurrentValue;
	float OriginMax = MaxValue;

	CurrentValue = NewCurrentValue;

	if (NewMaxValue < 0) MaxValue = 0;
	else MaxValue = NewMaxValue;

	if (CurrentValue > MaxValue) CurrentValue = MaxValue;

	if (OriginCurrent != CurrentValue || OriginMax != MaxValue) BroadcastDirty();
}

float UFillValue::BroadcastDirty()
{
	float Ratio;
	if (MaxValue <= 0) Ratio = 1;
	else Ratio = CurrentValue / MaxValue;
	OnValueChanged.Broadcast(CurrentValue, MaxValue, Ratio);
}
