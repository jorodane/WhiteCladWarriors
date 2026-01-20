// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionValueClaimer.h"
#include "Actions/ActionExecutor.h"

FVector FPositionClaimer::GetPosition(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if(!IsValid(Executor)) return FVector::ZeroVector;
	return Executor->GetSavedPosition(PositionTag, Component, ID);
}

FRotator FDirectionClaimer::GetRotator(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const
{
	if(!IsValid(Executor)) return FRotator::ZeroRotator;
	return Executor->GetSavedDirection(DirectionTag, Component, ID).ToOrientationRotator();
}

FVector FDirectionClaimer::GetDirection(const UActionExecutor* Executor, UUnitActionComponent* Component, const int ID) const
{
	if (!IsValid(Executor)) return FVector::ZeroVector;
	return Executor->GetSavedDirection(DirectionTag, Component, ID);
}

AActor* FActorClaimer::GetActor(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if(!IsValid(Executor)) return nullptr;
	return Executor->GetSavedActor(ActorTag, Component, ID);
}

TArray<AActor*> FActorArrayClaimer::GetActorArray(const UActionExecutor* Executor, const UUnitActionComponent* Component, const int ID) const
{
	if (!IsValid(Executor)) return TArray<AActor*>();
	return Executor->GetSavedActorArray(ActorArrayTag, Component, ID);
}

