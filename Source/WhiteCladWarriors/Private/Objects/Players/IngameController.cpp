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

FVector AIngameController::GetTerrainPosition(float HeightOffset)
{
	FVector Origin, Direction;
	if (!DeprojectMousePositionToWorld(Origin, Direction)) return FVector::ZeroVector;
	if (Direction.Z == 0) return FVector::ZeroVector;
	const float Distance = (HeightOffset - Origin.Z) / Direction.Z;

	return Origin + (Direction * Distance);
}

void AIngameController::SetCursor_Implementation(EMouseCursor::Type NewCursor)
{
	if (!NewCursor) NewCursor = EMouseCursor::Default;
	CurrentMouseCursor = NewCursor;
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

