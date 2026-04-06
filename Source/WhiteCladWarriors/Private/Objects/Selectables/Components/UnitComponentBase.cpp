// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Components/Widget.h"

void UUnitComponentBase::BeginDestroy()
{
	BroadcastRemoveMessage();
	Super::BeginDestroy();
}

void UUnitComponentBase::BroadcastRemoveMessage()
{
	OnComponentRemoved.Broadcast(this);
	OnComponentRemoved.Clear();
}

FVector UUnitComponentBase::GetLocation_Implementation()
{
	if (AActor* Owner = GetOwner()) return Owner->GetActorLocation();
	return FVector::ZeroVector;
};

FVector UUnitComponentBase::GetDirection_Implementation(FVector Destination, bool bIsIgnoreZ)
{
	FVector Result = Destination - GetLocation();
	if (bIsIgnoreZ) Result = Result.GetSafeNormal2D();
	else Result = Result.GetSafeNormal();
	return Result;
};

UUnitMainComponent* UUnitComponentBase::SetOwnerUnit_Implementation(UUnitMainComponent* NewUnit)
{
	return OwnerUnit = NewUnit;
}

bool UUnitComponentBase::TryGetOwnerUnit(UUnitMainComponent*& ResultUnit) const
{
	ResultUnit = GetOwnerUnit();
	return IsValid(ResultUnit);
}
