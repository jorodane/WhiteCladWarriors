// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Generals/Widgets/GenericWidgetClaimer.h"

UGenericWidgetClaim::UGenericWidgetClaim()
{
	Initialize();
}

void UGenericWidgetClaim::AssignGenericWidgetEvents(const FOnWidgetConnectedSingle& Connected, const FOnWidgetUpdatedSingle& Updated)
{
	if (Connected.IsBound()) OnWidgetConnected.AddUnique(Connected);
	if (Updated.IsBound()) OnWidgetUpdated.AddUnique(Updated);
}