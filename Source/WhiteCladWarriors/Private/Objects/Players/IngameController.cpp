// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Players/IngameController.h"
#include "Objects/Players/Operator.h"
#include "Framework/Application/NavigationConfig.h"

void AIngameController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (IsLocalPlayerController())
	{
		NavConfig = MakeShared<FNavigationConfig>();
		if (NavConfig.IsValid())
		{
			NavConfig->bTabNavigation = false;
			NavConfig->bKeyNavigation = false;
			NavConfig->bAnalogNavigation = false;
			FSlateApplication::Get().SetNavigationConfig(NavConfig->AsShared());
		}
	}

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

void AIngameController::SetCursor_Implementation(EInputMouseCursorType NewCursor)
{
	switch (NewCursor)
	{
	default:
	case EInputMouseCursorType::Default:	CurrentMouseCursor = EMouseCursor::Type::Default;			break;

	case EInputMouseCursorType::Up:			CurrentMouseCursor = EMouseCursor::Type::ResizeLeftRight;	break;
	case EInputMouseCursorType::Down:		CurrentMouseCursor = EMouseCursor::Type::ResizeUpDown;		break;
	case EInputMouseCursorType::Right:		CurrentMouseCursor = EMouseCursor::Type::ResizeSouthEast;	break;
	case EInputMouseCursorType::Left:		CurrentMouseCursor = EMouseCursor::Type::ResizeSouthWest;	break;
	case EInputMouseCursorType::Work:		CurrentMouseCursor = EMouseCursor::Type::CardinalCross;		break;
	case EInputMouseCursorType::Grab:		CurrentMouseCursor = EMouseCursor::Type::Hand;				break;
	case EInputMouseCursorType::Selectable: CurrentMouseCursor = EMouseCursor::Type::GrabHand;			break;
	case EInputMouseCursorType::Impossible: CurrentMouseCursor = EMouseCursor::Type::SlashedCircle;		break;
	case EInputMouseCursorType::Attack:		CurrentMouseCursor = EMouseCursor::Type::Crosshairs;		break;
	case EInputMouseCursorType::Rotate:		CurrentMouseCursor = EMouseCursor::Type::EyeDropper;		break;
	case EInputMouseCursorType::Target:		CurrentMouseCursor = EMouseCursor::Type::GrabHandClosed;	break;
	}
	FSlateApplication::Get().QueryCursor();
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

