// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Values/ValueObject.h"
#include "Objects/Values/ValueContainer.h"


void UValueObject::ContainerConnect(UValueContainer* NewContainer)
{
	ConnectedContainer = NewContainer;

	OnContainerConnected(ConnectedContainer);
}

void UValueObject::ContainerDisconnect()
{
	UValueContainer* OldContainer = ConnectedContainer;
	ConnectedContainer = nullptr;
	if (!IsValid(OldContainer)) return;
	OnContainerDisconnected(OldContainer);
}