// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Components/Widget.h"

void UUnitComponentBase::BeginDestroy()
{
	BroadcastMessage_Removed();
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
		OwnerUnit->UnitMessage_Simple(Message);
	}
}

void UUnitComponentBase::BroadcastMessage_Detail(const FName& Message, const FName& Context)
{
	if (IsValid(OwnerUnit))
	{
		OwnerUnit->UnitMessage_Detail(Message, Context);
	}
}
void UUnitComponentBase::BroadcastMessage_Montage(UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted)
{
	if (IsValid(OwnerUnit))
	{
		OwnerUnit->UnitMessage_Montage(Montage, bIsStart, bIsInterrupted);
	}
}


void UUnitComponentBase::ReceiveUnitMessage_Simple(const FName& Message)
{
	OnComponentMessage_Simple.Broadcast(this, Message);
}

void UUnitComponentBase::ReceiveUnitMessage_Detail(const FName& Message, const FName& Context)
{
	OnComponentMessage_Detail.Broadcast(this, Message, Context);
}

void UUnitComponentBase::ReceiveUnitMessage_Montage(UAnimMontage* Montage, bool bIsStart, bool bIsInterrupted)
{
	OnComponentMessage_Montage.Broadcast(this, Montage, bIsStart, bIsInterrupted);
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

AOperator* UUnitComponentBase::GetOperator_Implementation() const
{
	if (!IsValid(OwnerUnit)) return nullptr; 
	return OwnerUnit->GetOperator();
}


UUnitMainComponent* UUnitComponentBase::SetOwnerUnit_Implementation(UUnitMainComponent* NewUnit)
{
	return OwnerUnit = NewUnit;
}

bool UUnitComponentBase::TryGetOwnerUnit(UUnitMainComponent*& ResultUnit) const
{
	ResultUnit = GetOwnerUnit();
	return IsValid(ResultUnit);
}
