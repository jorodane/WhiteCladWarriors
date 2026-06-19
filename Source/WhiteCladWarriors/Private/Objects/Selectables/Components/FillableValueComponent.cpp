
#include "Objects/Selectables/Components/FillableValueComponent.h"

float UFillableValueComponent::GetPercent_Implementation() const
{
	if (MaxValue <= 0) return 1;
	else return CurrentValue / MaxValue;
}
float UFillableValueComponent::SetPercent_Implementation(float NewValue)
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

float UFillableValueComponent::SetCurrentValue_Implementation(float NewValue)
{
	if (NewValue != CurrentValue)
	{
		CurrentValue = NewValue;
		BroadcastDirty();
	}
	return CurrentValue;
}

float UFillableValueComponent::SetMaxValue_Implementation(float NewValue)
{
	if (NewValue != MaxValue)
	{
		MaxValue = NewValue;
		BroadcastDirty();
	}
	return MaxValue;
}

float UFillableValueComponent::SetValue_Implementation(float NewCurrentValue, float NewMaxValue)
{
	float OriginCurrent = CurrentValue;
	float OriginMax = MaxValue;

	CurrentValue = NewCurrentValue;

	if (NewMaxValue < 0) MaxValue = 0;
	else MaxValue = NewMaxValue;

	if (CurrentValue > MaxValue) CurrentValue = MaxValue;

	if (OriginCurrent != CurrentValue || OriginMax != MaxValue) return BroadcastDirty();
	else return GetPercent();
}

float UFillableValueComponent::AddValue_Implementation(float Value)
{
	if (Value == 0) return 0;
	else if (Value > 0)
	{
		float Overflow = FMath::Max(0, Value - GetFillableValue());
		Value -= Overflow;
		if (Value == 0) return 0;
		CurrentValue += Value;
		if (GetIsFull())
		{
			OnValueFull();
			if (Overflow > 0) OnValueOverflow(Overflow);
		}
	}
	else if (Value < 0)
	{
		float UnderFlow = FMath::Max(0, Value - CurrentValue);
		Value += UnderFlow;
		if (Value == 0) return 0;
		CurrentValue += Value;
		if (GetIsEmpty())
		{
			OnValueEmpty();
			if (UnderFlow > 0) OnValueUnderflow(UnderFlow);
		}
	}
	BroadcastDirty();
	return Value;
}

float UFillableValueComponent::BroadcastDirty_Implementation()
{
	float Ratio = GetPercent();
	OnValueChanged.Broadcast(CurrentValue, MaxValue, Ratio);
	return Ratio;
}
