// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionIndicatorBase.h"
#include "Actions/ActionIndicatorShowerBase.h"
#include "Actions/ActionBehaviorNode.h"
#include "Actions/ActionSelectorNode.h"
#include "Generals/Structs/ActionStructures.h"
#include "Objects/Generals/Components/PoolComponent.h"
#include "Objects/Players/Operator.h"

void UActionIndicatorBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetOwnerOperator(Cast<AOperator>(GetOwner()));

	for (auto& CurrentPoolPair : PoolDefaultClassMap)
	{
		EInputIndicatorType WantType = CurrentPoolPair.Key;
		TSubclassOf<AActionIndicatorShowerBase> WantClass = CurrentPoolPair.Value;

		if (IsValid(WantClass))
		{
			UPoolComponent* NewComponent = NewObject<UPoolComponent>(this);
			if (!IsValid(NewComponent)) continue;
			PoolComponentMap.Add(WantType, NewComponent);
			NewComponent->Initialize(WantClass);
		}
	}
}

void UActionIndicatorBase::SetOwnerOperator_Implementation(AOperator* NewOperator)
{
	OwnerOperator = NewOperator;
	if (IsValid(OwnerOperator))
	{
		OwnerOperator->OnInputClaimChanged.AddDynamic(this, &UActionIndicatorBase::ReceiveInputClaim);
	}
}

void UActionIndicatorBase::SetIndicator_Implementation(UActionExecutor* TargetExecutor, const TArray<UUnitActionComponent*>& TargetComponents, UActionSelectorNode* TargetNode)
{
	


}

void UActionIndicatorBase::SetVisible_Implementation()
{
	bIsActivated = true;


}

void UActionIndicatorBase::SetInvisible_Implementation()
{
	bIsActivated = false;
}

void UActionIndicatorBase::ReceiveInputClaim_Implementation(const FInputClaim& NewClaim, bool ValidClaim)
{
	CurrentExecutor = NewClaim.TargetActionCursor.CurrentExecutor;
	CurrentNode = NewClaim.TargetNode;
	CurrentComponents = NewClaim.TargetComponentArray;

	for (UActionBehaviorNode* CurrentRequestNode : CurrentNode->GetIndicatorNodes()) ShowerActiveMap.Add(CurrentRequestNode);

	for (auto& CurrentActiveMap : ShowerActiveMap)
	{
		UActionBehaviorNode* CurrentBehavior = CurrentActiveMap.Key;
		if (!IsValid(CurrentBehavior)) continue;
		TSet<AActionIndicatorShowerBase*>& CurrentSet = CurrentActiveMap.Value;
		FIndicatorClaim CurrentRequest = CurrentBehavior->GetIndicatorClaim(NewClaim);
		UPoolComponent** CurrentFinder = PoolComponentMap.Find(CurrentRequest.IndicatorType);
		if (CurrentFinder == nullptr) continue;
		UPoolComponent* CurrentPool = *CurrentFinder;
		for (int i = 0; i < CurrentRequest.Amount; i++)
		{
			TSoftObjectPtr<AActor> CurrentShower = CurrentPool->DequeueInstance();
			if (!CurrentShower.IsValid()) continue;
			AActionIndicatorShowerBase* NewShower = Cast<AActionIndicatorShowerBase>(CurrentShower.Get());
			CurrentSet.Add(NewShower);
		}
	}
}