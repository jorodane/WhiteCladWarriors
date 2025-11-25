// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitComponentBase.h"

TArray<UWidget*> UUnitComponentBase::GetConnectedWidgetsWithTag(FName WantTag)
{
	FWidgetArray* Result = ConnectedWidgets.Find(WantTag);
	if (Result) return Result->WidgetArray;
	return TArray<UWidget*>();
}

void UUnitComponentBase::ConnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag)
{
	IInfoConnectable::Execute_OnConnectInfoWidget(this, TargetWidget, ClaimedTag);
	FWidgetArray& Array = ConnectedWidgets.FindOrAdd(ClaimedTag);
	Array.WidgetArray.Add(TargetWidget);
};

void UUnitComponentBase::DisconnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag)
{
	IInfoConnectable::Execute_OnDisconnectInfoWidget(this, TargetWidget, ClaimedTag);

	if (FWidgetArray* Array = ConnectedWidgets.Find(ClaimedTag)) Array->WidgetArray.Remove(TargetWidget);
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

