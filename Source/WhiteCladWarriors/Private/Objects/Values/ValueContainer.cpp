// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Values/ValueContainer.h"
#include "Objects/Values/ValueObject.h"
#include "Objects/Values/FloatValue.h"
#include "Objects/Values/FillableValue.h"

float UValueContainer::SetNumber(FName WantTag, float Value)
{
	UFloatValue* CurrentValue = FindOrAddNumberObject(WantTag, 0.0f, UFloatValue::StaticClass());
	return CurrentValue->SetValue(Value);
}

float UValueContainer::AddNumber(FName WantTag, float Value)
{
	UFloatValue* CurrentValue = FindOrAddNumberObject(WantTag, 0.0f, UFloatValue::StaticClass());
	return CurrentValue->AddValue(Value);
}

float UValueContainer::GetNumber(FName WantTag, float DefaultValue)
{
	UFloatValue* CurrentValue;
	if (TryFindNumberObject(WantTag, CurrentValue)) return CurrentValue->GetValue();
	return DefaultValue;
}


UFloatValue* UValueContainer::AddNumberObject(FName WantTag, float InitialValue, TSubclassOf<UFloatValue> Template)
{
	if (!Template) return nullptr;

	if (!NumberValueMap.Contains(WantTag))
	{
		UFloatValue* Instance = NewObject<UFloatValue>(this, Template);
		if (!IsValid(Instance)) return nullptr;
		Instance->SetValue(InitialValue);
		Instance->OnContainerConnected(this);
		NumberValueMap.Add(WantTag, Instance);
		return Instance;
	}
	return nullptr;
}

UFillableValue* UValueContainer::AddFillableObject(FName WantTag, float InitialValue, float MaxValue, TSubclassOf<UFillableValue> Template)
{
	if (!Template) return nullptr;

	if (!NumberValueMap.Contains(WantTag))
	{
		UFillableValue* Instance = NewObject<UFillableValue>(this, Template);
		if (!IsValid(Instance)) return nullptr;
		Instance->SetFillValue(InitialValue, MaxValue);
		Instance->OnContainerConnected(this);
		NumberValueMap.Add(WantTag, Instance);
		return Instance;
	}
	return nullptr;
}

void UValueContainer::RemoveNumberObject(FName WantTag)
{
	NumberValueMap.Remove(WantTag);
}

TArray<UValueObject*> UValueContainer::FindAllValueOjbect() const
{
	TArray<UValueObject*> Result;
	for (const auto& CurrentPair : NumberValueMap)
	{
		UValueObject* CurrentObject = CurrentPair.Value;
		if (IsValid(CurrentObject)) Result.Add(CurrentObject);
	}
	return Result;
}

UFloatValue* UValueContainer::FindOrAddNumberObject(FName WantTag, float DefaultValue, TSubclassOf<UFloatValue> Template)
{
	UFloatValue* Result;
	if (TryFindNumberObject(WantTag, Result)) return Result;
	return AddNumberObject(WantTag, DefaultValue, Template);
}

UFloatValue* UValueContainer::FindNumberObject(FName WantTag) const
{
	const TObjectPtr<UFloatValue>* Result = NumberValueMap.Find(WantTag);
	return Result ? *Result : nullptr;
}

bool UValueContainer::TryFindNumberObject(FName WantTag, UFloatValue*& Result)
{
	Result = FindNumberObject(WantTag);
	return IsValid(Result);
}

UFillableValue* UValueContainer::FindFillableObject(FName WantTag) const
{
	return Cast<UFillableValue>(FindNumberObject(WantTag));
}

bool UValueContainer::TryFindFillableObject(FName WantTag, UFillableValue*& Result)
{
	Result = FindFillableObject(WantTag);
	return Result != nullptr;
}

TArray<UOrderedGenericWidgetClaim*> UValueContainer::GetInfoWidget_Implementation(EInfoWidgetType WantType, AOperator* Operator) const
{
	TArray<UOrderedGenericWidgetClaim*> Result;
	for (UValueObject* CurrentComponent : FindAllValueOjbect())
	{
		if (CurrentComponent && CurrentComponent->GetClass()->ImplementsInterface(UInfoConnectable::StaticClass()))
		{
			Result.Append(IInfoConnectable::Execute_GetInfoWidget(CurrentComponent, WantType, Operator));
		}
	};
	return Result;
}





UValueContainer* UValueContainer::GetValueContainer(AActor* From)
{
	if (!IsValid(From)) return nullptr;
	return From->GetComponentByClass<UValueContainer>();
}

bool UValueContainer::TryGetValueContainer(AActor* From, UValueContainer*& Result)
{
	Result = GetValueContainer(From);
	return IsValid(Result);
}

UValueContainer* UValueContainer::GetOrAddValueContainer(AActor* From)
{
	if (!IsValid(From)) return nullptr;
	UValueContainer* Result = GetValueContainer(From);
	if (IsValid(Result)) return Result;
	Result = NewObject<UValueContainer>(From);
	if (!IsValid(Result)) return nullptr;
	From->AddInstanceComponent(Result);
	Result->RegisterComponent();
	return Result;
}

