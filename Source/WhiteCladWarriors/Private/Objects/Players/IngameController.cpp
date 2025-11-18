// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Players/IngameController.h"
#include "Objects/Players/Operator.h"

void AIngameController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (AOperator* asOperator = Cast<AOperator>(aPawn))
	{
		if (asOperator == ConnectedOperator) return;

		OnOperatorConnected(asOperator);
	}
}

void AIngameController::OnOperatorConnected_Implementation(AOperator* NewOperator)
{
	if (ConnectedOperator) OnOperatorDisconnected(ConnectedOperator);

	ConnectedOperator = NewOperator;
	
	if (ConnectedOperator)
	{
		IPlayerConnectable::Execute_OnPlayerConnected(ConnectedOperator, this);
	}
}

void AIngameController::OnOperatorDisconnected_Implementation(AOperator* OldOperator)
{
	if (IsValid(OldOperator))
	{
		IPlayerConnectable::Execute_OnPlayerDisconnected(ConnectedOperator, this);
	}

	if (OldOperator == ConnectedOperator) ConnectedOperator = nullptr;
}

