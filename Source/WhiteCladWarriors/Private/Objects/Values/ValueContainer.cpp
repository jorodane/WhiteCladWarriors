// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Values/ValueContainer.h"
#include "Objects/Values/ValueObject.h"
#include "Objects/Values/FloatValue.h"
#include "Objects/Values/FillableValue.h"

float UValueContainer::SetNumber(FName WantTag, float Value)
{
	UFloatValue* CurrentValue = FindOrAddNumberObject(WantTag, UFloatValue::StaticClass());
	return CurrentValue->SetValue(Value);
}

float UValueContainer::AddNumber(FName WantTag, float Value)
{
	UFloatValue* CurrentValue = FindOrAddNumberObject(WantTag, UFloatValue::StaticClass());
	return CurrentValue->AddValue(Value);
}

float UValueContainer::GetNumber(FName WantTag, float DefaultValue)
{
	UFloatValue* CurrentValue;
	if (TryFindNumberObject(WantTag, CurrentValue)) return CurrentValue->GetValue();
	return DefaultValue;
}


UFloatValue* UValueContainer::AddNumberObject(FName WantTag, TSubclassOf<UFloatValue> Template)
{
	if (!Template) return nullptr;

	if (!NumberValueMap.Contains(WantTag))
	{
		UFloatValue* Instance = NewObject<UFloatValue>(this, Template);
		NumberValueMap.Add(WantTag, Instance);
		return Instance;
	}
	return nullptr;
}

void UValueContainer::RemoveNumberObject(FName WantTag)
{
	NumberValueMap.Remove(WantTag);
}

TArray<UFloatValue*> UValueContainer::FindAllValueOjbect()
{
	TArray<TObjectPtr<UFloatValue>> Result;
	NumberValueMap.GenerateValueArray(Result);
	return Result;
}

UFloatValue* UValueContainer::FindOrAddNumberObject(FName WantTag, TSubclassOf<UFloatValue> Template)
{
	UFloatValue* Result;
	if (TryFindNumberObject(WantTag, Result)) return Result;
	return AddNumberObject(WantTag, Template);
}

UFloatValue* UValueContainer::FindNumberObject(FName WantTag)
{
	TObjectPtr<UFloatValue>* Result = NumberValueMap.Find(WantTag);
	return Result ? *Result : nullptr;
}

bool UValueContainer::TryFindNumberObject(FName WantTag, UFloatValue*& Result)
{
	Result = FindNumberObject(WantTag);
	return IsValid(Result);
}

UFillableValue* UValueContainer::FindFillableValue(FName WantTag)
{
	return Cast<UFillableValue>(FindNumberObject(WantTag));
}

bool UValueContainer::TryFindFillableValue(FName WantTag, UFillableValue*& Result)
{
	Result = FindFillableValue(WantTag);
	return Result != nullptr;
}





UValueContainer* UValueContainer::GetValueContainer(AActor* From)
{
	if (!IsValid(From)) return nullptr;
	return From->GetComponentByClass<UValueContainer>();
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

