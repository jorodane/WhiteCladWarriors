// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionIndicatorBase.h"
#include "Actions/ActionIndicatorShowerBase.h"
#include "Actions/ActionBehaviorNode.h"
#include "Actions/ActionSelectorNode.h"
#include "Generals/Structs/ActionStructures.h"
#include "Generals/Structs/InputPackage.h"
#include "Objects/Generals/Components/PoolComponent.h"
#include "Objects/Players/Operator.h"

UActionIndicatorBase::UActionIndicatorBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

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
void UActionIndicatorBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UActionIndicatorBase::SetOwnerOperator_Implementation(AOperator* NewOperator)
{
	OwnerOperator = NewOperator;
	if (IsValid(OwnerOperator))
	{
		OwnerOperator->OnInputClaimChanged.AddDynamic(this, &UActionIndicatorBase::ReceiveInputClaim);
	}
}

void UActionIndicatorBase::SetVisible_Implementation()
{
	bIsActivated = true;
}

void UActionIndicatorBase::SetInvisible_Implementation()
{
	bIsActivated = false;
	CurrentClaim.Clear();
	CurrentExecutor = nullptr;
	CurrentNode = nullptr;
	CurrentComponents.Empty();

	for (auto& CurrentPoolPair : PoolComponentMap)
	{
		UPoolComponent* CurrentPool = CurrentPoolPair.Value;
		if (!IsValid(CurrentPool)) continue;
		CurrentPool->EnqueueAll();
	}
	ShowerActiveMap.Empty();
}

void UActionIndicatorBase::ReceiveInputClaim_Implementation(const FInputClaim& NewClaim, bool ValidClaim)
{
	if (!ValidClaim)
	{
		SetInvisible();
		return;
	}

	CurrentClaim = NewClaim;
	CurrentExecutor = NewClaim.TargetActionCursor.CurrentExecutor;
	CurrentNode = NewClaim.TargetNode;
	CurrentComponents = NewClaim.TargetComponentArray;
	bIsActivated = true;

	for (UActionBehaviorNode* CurrentRequestNode : CurrentNode->GetIndicatorNodes()) ShowerActiveMap.Add(CurrentRequestNode);

	for (auto& CurrentActivePair : ShowerActiveMap)
	{
		UActionBehaviorNode* CurrentBehavior = CurrentActivePair.Key;
		if (!IsValid(CurrentBehavior)) continue;
		TArray<AActionIndicatorShowerBase*>& CurrentSet = CurrentActivePair.Value;
		FIndicatorClaim CurrentRequest = CurrentBehavior->GetIndicatorClaim(NewClaim);
		UPoolComponent** CurrentFinder = PoolComponentMap.Find(CurrentRequest.IndicatorType);
		if (CurrentFinder == nullptr) continue;
		UPoolComponent* CurrentPool = *CurrentFinder;
		for (int i = 0; i < CurrentRequest.Amount; i++)
		{
			TSoftObjectPtr<AActor> CurrentShower = CurrentPool->DequeueInstance();
			if (!CurrentShower.IsValid()) continue;
			AActionIndicatorShowerBase* NewShower = Cast<AActionIndicatorShowerBase>(CurrentShower.Get());
			CurrentSet.AddUnique(NewShower);
		}
	}
}

void UActionIndicatorBase::UpdateShower_Implementation()
{
	if (CurrentClaim.TargetNode == nullptr || !IsValid(OwnerOperator)) return;

	const FInputPackage& Input = OwnerOperator->GetInputPackage();

	for (auto& CurrentActivePair : ShowerActiveMap)
	{
		UActionBehaviorNode* CurrentRequestNode = CurrentActivePair.Key;
		TArray<AActionIndicatorShowerBase*> CurrentShowerSet = CurrentActivePair.Value;

		if (!IsValid(CurrentRequestNode)) continue;

		CurrentRequestNode->UpdateIndicator(CurrentClaim, Input, CurrentShowerSet);
	}
}