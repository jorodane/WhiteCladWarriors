// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Values/ValueContainer.h"

float UValueContainer::SetFloatValue(FName WantTag, float Value)
{
	return FloatValueMap.Add(WantTag, Value);
}

float UValueContainer::AddFloatValue(FName WantTag, float Value)
{
	float& Map = FloatValueMap.FindOrAdd(WantTag, 0);
	return Map += Value;
}

float UValueContainer::GetFloatValue(FName WantTag, float DefaultValue)
{
	return FloatValueMap.FindOrAdd(WantTag, DefaultValue);
}


bool UValueContainer::AddFillableValue(FName WantTag, UFillableValue* Target)
{
	if (!FillValueMap.Contains(WantTag))
	{
		FillValueMap.Add(WantTag, Target);
		return true;
	}
	return false;
}

void UValueContainer::RemoveFillableValue(FName WantTag)
{
	UFillableValue** Finder = FillValueMap.Find(WantTag);
	if (Finder)
	{
		FillValueMap.Remove(WantTag);
	}
}

UFillableValue* UValueContainer::FindFillableValue(FName WantTag)
{
	UFillableValue** Finder = FillValueMap.Find(WantTag);
	if (Finder) return *Finder;
	else return nullptr;
}

bool UValueContainer::TryFindFillableValue(FName WantTag, UFillableValue*& Result)
{
	Result = FindFillableValue(WantTag);
	return Result != nullptr;
}

TArray<UFillableValue*> UValueContainer::FindAllFillableValue()
{
	TArray<UFillableValue*> Result;
	FillValueMap.GenerateValueArray(Result);
	return Result;
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

