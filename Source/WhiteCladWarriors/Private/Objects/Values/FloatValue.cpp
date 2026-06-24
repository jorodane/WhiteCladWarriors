
#include "Objects/Values/FloatValue.h"


float UFloatValue::SetValue_Implementation(float NewValue)
{
	float OriginCurrent = CurrentValue;
	CurrentValue = NewValue;
	if (OriginCurrent != CurrentValue) return BroadcastDirty();
	else return CurrentValue;
}

float UFloatValue::AddValue_Implementation(float Value)
{
	if (Value == 0) return 0;
	CurrentValue += Value;
	BroadcastDirty();
	return Value;
}

float UFloatValue::BroadcastDirty_Implementation()
{
	OnCurrentValueChanged.Broadcast(CurrentValue);
	return CurrentValue;
}
