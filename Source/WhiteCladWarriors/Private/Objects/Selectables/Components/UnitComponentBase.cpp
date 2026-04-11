// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Components/Widget.h"

void UUnitComponentBase::BeginDestroy()
{
	BroadcastMessage_Removed();
	if (IsValid(OwnerUnit))
	{
		OwnerUnit->OnUnitMessage_Simple.RemoveAll(this);
	}
	Super::BeginDestroy();
}

void UUnitComponentBase::BroadcastMessage_Removed()
{
	OnComponentRemoved.Broadcast(this);
	OnComponentRemoved.Clear();
}

void UUnitComponentBase::BroadcastMessage_Simple(const FName& Message)
{
	if (IsValid(OwnerUnit))
	{
		OwnerUnit->UnitMessage(Message);
	}
}

void UUnitComponentBase::ReceiveUnitMessage_Simple(const FName& Message)
{
	OnComponentMessage_Simple.Broadcast(this, Message);
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
	if (IsValid(OwnerUnit))
	{
		OwnerUnit->OnUnitMessage_Simple.RemoveAll(this);
	}
	if (IsValid(NewUnit))
	{
		NewUnit->OnUnitMessage_Simple.AddUniqueDynamic(this, &UUnitComponentBase::ReceiveUnitMessage_Simple);
	}
	return OwnerUnit = NewUnit;
}

bool UUnitComponentBase::TryGetOwnerUnit(UUnitMainComponent*& ResultUnit) const
{
	ResultUnit = GetOwnerUnit();
	return IsValid(ResultUnit);
}
