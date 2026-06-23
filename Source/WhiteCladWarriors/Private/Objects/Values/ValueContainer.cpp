// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Values/ValueContainer.h"

UFillableValue* UValueContainer::AddFillableValue()
{
	return nullptr;
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
