// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitComponentBase.h"
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