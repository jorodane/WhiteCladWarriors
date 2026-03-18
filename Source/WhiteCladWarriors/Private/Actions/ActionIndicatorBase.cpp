// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionIndicatorBase.h"
#include "Objects/Players/Operator.h"

void UActionIndicatorBase::SetOwner_Implementation(AOperator* NewOperator)
{
	OwnerOperator = NewOperator; 
	if (IsValid(OwnerOperator))
	{
		OwnerOperator->OnInputClaimChanged.AddDynamic(this, &UActionIndicatorBase::ReceiveInputClaim);
	}
}

void UActionIndicatorBase::SetIndicator_Implementation(UActionExecutor* TargetExecutor, const TArray<UUnitActionComponent*>& TargetComponents, UActionSelectorNode* StartNode)
{

}

void UActionIndicatorBase::SetVisible_Implementation()
{

}

void UActionIndicatorBase::SetInvisible_Implementation()
{

}

void UActionIndicatorBase::ReceiveInputClaim_Implementation(const FInputClaim& NewClaim, bool ValidClaim)
{

}

AActionIndicatorShowerBase* UActionIndicatorBase::ShowerPop()
{
	return nullptr;
}
void UActionIndicatorBase::ShowerPush(EInputIndicatorType WantType, AActionIndicatorShowerBase* NewShower)
{

}
