// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitComponentBase.h"

void UUnitComponentBase::ConnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag)
{
	IInfoConnectable::Execute_OnConnectInfoWidget(this, TargetWidget, ClaimedTag);
};

void UUnitComponentBase::DisconnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag)
{
	IInfoConnectable::Execute_OnDisconnectInfoWidget(this, TargetWidget, ClaimedTag);
};



//// Sets default values for this component's properties
//UUnitComponentBase::UUnitComponentBase()
//{
//	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
//	// off to improve performance if you don't need them.
//	PrimaryComponentTick.bCanEverTick = true;
//
//	// ...
//}
//
//
//// Called when the game starts
//void UUnitComponentBase::BeginPlay()
//{
//	Super::BeginPlay();
//
//	// ...
//	
//}
//
//
//// Called every frame
//void UUnitComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

