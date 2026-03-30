// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitComponentBase.h"
#include "Components/Widget.h"

void UUnitComponentBase::BeginDestroy()
{
	BroadcastRemoveMessage();
	Super::BeginDestroy();
}

TArray<UWidget*> UUnitComponentBase::GetConnectedWidgetsWithTag(FName WantTag)
{
	FWidgetArray* Result = ConnectedWidgets.Find(WantTag);
	if (Result) return Result->WidgetArray;
	return TArray<UWidget*>();
}

void UUnitComponentBase::ConnectInfoWidget_Implementation(EInfoWidgetType NewType, UWidget* TargetWidget, FName ClaimedTag)
{
	IInfoConnectable::Execute_OnConnectInfoWidget(this, NewType, TargetWidget, ClaimedTag);
	if (TargetWidget == nullptr) return;
	FWidgetArray& Array = ConnectedWidgets.FindOrAdd(ClaimedTag);
	Array.WidgetArray.Add(TargetWidget);
};

void UUnitComponentBase::DisconnectInfoWidget_Implementation(EInfoWidgetType OldType, UWidget* TargetWidget, FName ClaimedTag)
{
	IInfoConnectable::Execute_OnDisconnectInfoWidget(this, OldType, TargetWidget, ClaimedTag);

	if (TargetWidget == nullptr) return;
	if (FWidgetArray* Array = ConnectedWidgets.Find(ClaimedTag)) Array->WidgetArray.Remove(TargetWidget);
};

void UUnitComponentBase::BroadcastRemoveMessage()
{
	OnComponentRemoved.Broadcast(this);
	OnComponentRemoved.Clear();
}