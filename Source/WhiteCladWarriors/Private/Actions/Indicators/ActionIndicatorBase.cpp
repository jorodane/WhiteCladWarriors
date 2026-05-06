// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Indicators/ActionIndicatorBase.h"
#include "Actions/Indicators/ActionIndicatorShowerBase.h"
#include "Actions/Executables/ActionBehaviorNode.h"
#include "Actions/Executables/ActionSelectorNode.h"
#include "Actions/ActionBase.h"
#include "Actions/UnitActionContainer.h"
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

	UpdateShower(bIsTriggerByIcon);
}

void UActionIndicatorBase::ResetIndicator()
{
	ClearShower();
	InitializePool();
}

void UActionIndicatorBase::SetOwnerOperator_Implementation(AOperator* NewOperator)
{
	OwnerOperator = NewOperator;
	if (IsValid(OwnerOperator))
	{
		OwnerOperator->OnInputClaimChanged.AddDynamic(this, &UActionIndicatorBase::ReceiveInputClaim);
	}
}

void UActionIndicatorBase::ClearShower_Implementation()
{
	bIsActivated = false;
	bIsTriggerByIcon = false;
	CurrentClaim.Clear();
	CurrentExecutor = nullptr;
	CurrentNode = nullptr;
	CurrentComponents.Empty();

	InitializePool();
}

void UActionIndicatorBase::ReceiveInputClaim_Implementation(const FInputClaim& NewClaim, bool ValidClaim, bool TriggerByIcon)
{
	UActionSelectorNode* NewNode = NewClaim.TargetNode;
	if (!ValidClaim || !IsValid(NewNode))
	{
		ResetIndicator();
		return;
	}
	else if (bIsActivated)
	{
		ResetIndicator();
	}
	CurrentExecutor = NewClaim.TargetActionCursor.CurrentExecutor;
	CurrentComponents = NewClaim.TargetComponentArray;
	CurrentNode = NewNode;
	CurrentClaim = NewClaim;
	bIsActivated = true;
	bIsTriggerByIcon = TriggerByIcon;
	TArray<UActionBehaviorNode*> IndicatorNodes = NewNode->GetIndicatorNodes();

	for (UActionBehaviorNode* CurrentRequestNode : IndicatorNodes) ShowerActiveMap.Add(CurrentRequestNode);

	for (auto& CurrentActivePair : ShowerActiveMap)
	{
		UActionBehaviorNode* CurrentBehavior = CurrentActivePair.Key;
		if (!IsValid(CurrentBehavior)) continue;
		TArray<AActionIndicatorShowerBase*>& CurrentArray = CurrentActivePair.Value;
		TMap<UActionBehaviorNode*, FIndicatorClaim> CurrentRequestMap = CurrentBehavior->GetIndicatorClaim(NewClaim);
		for (const TPair<UActionBehaviorNode*, FIndicatorClaim>& CurrentRequestPair : CurrentRequestMap)
		{
			const FIndicatorClaim& CurrentRequest = CurrentRequestPair.Value;
			UPoolComponent** CurrentFinder = PoolComponentMap.Find(CurrentRequest.IndicatorType);
			if (CurrentFinder == nullptr) continue;
			UPoolComponent* CurrentPool = *CurrentFinder;
			for (int i = 0; i < CurrentRequest.Amount; i++)
			{
				TSoftObjectPtr<AActor> CurrentShower = CurrentPool->DequeueInstance();
				if (!CurrentShower.IsValid()) continue;
				AActionIndicatorShowerBase* NewShower = Cast<AActionIndicatorShowerBase>(CurrentShower.Get());
				CurrentArray.AddUnique(NewShower);
			}
		}
	}
}

void UActionIndicatorBase::ReceiveAction_Implementation(AActionBase* NewAction)
{
	UActionSelectorNode* SelecterNode;
	if (bIsActivated)
	{
		if (bIsTriggerByIcon) ResetIndicator();
		else return;
	}
	if (!IsValid(NewAction) || !IsValid(OwnerOperator) || !NewAction->IsRootNodeSelector(SelecterNode))
	{
		if (bIsTriggerByIcon) ResetIndicator();
		else return;
	}
	
	FInputClaim ClaimTemp;
	FActionCursorFinder& Cursor = ClaimTemp.TargetActionCursor;
	Cursor.CurrentAction = NewAction;
	Cursor.CurrentOperator = OwnerOperator;
	ClaimTemp.TargetComponentArray = OwnerOperator->GetAvailableComponentList(NewAction);
	ClaimTemp.TargetNode = SelecterNode;
	ReceiveInputClaim(ClaimTemp, true, true);
}

void UActionIndicatorBase::ReceiveActionContainer_Implementation(UUnitActionContainer* NewContainer)
{
	UActionSelectorNode* SelecterNode;

	if (bIsActivated || !IsValid(NewContainer) || !IsValid(OwnerOperator))
	{
		if (bIsTriggerByIcon) ResetIndicator();
		else return;
	}
	AActionBase* NewAction = NewContainer->CurrentAction;
	if (!IsValid(NewAction)  || !NewAction->IsRootNodeSelector(SelecterNode))
	{
		if (bIsTriggerByIcon) ResetIndicator();
		else return;
	}

	FInputClaim ClaimTemp;
	FActionCursorFinder& Cursor = ClaimTemp.TargetActionCursor;
	Cursor.CurrentAction = NewAction;
	Cursor.CurrentOperator = OwnerOperator;
	ClaimTemp.TargetComponentArray = NewContainer->CurrentComponents;
	ClaimTemp.TargetNode = SelecterNode;
	ReceiveInputClaim(ClaimTemp, true, true);
}

void UActionIndicatorBase::UpdateShower_Implementation(bool bIsIconPreview)
{
	if (CurrentClaim.TargetNode == nullptr || !IsValid(OwnerOperator)) return;

	const FInputPackage& Input = OwnerOperator->GetInputPackage();

	for (auto& CurrentActivePair : ShowerActiveMap)
	{
		UActionBehaviorNode* CurrentRequestNode = CurrentActivePair.Key;
		TArray<AActionIndicatorShowerBase*>& CurrentShowerArray = CurrentActivePair.Value;

		if (!IsValid(CurrentRequestNode)) continue;
		TMap<UActionBehaviorNode*, FIndicatorClaim> CurrentRequestMap = CurrentRequestNode->GetIndicatorClaim(CurrentClaim);
		for (const TPair<UActionBehaviorNode*, FIndicatorClaim>& CurrentRequestPair : CurrentRequestMap)
		{
			const FIndicatorClaim& CurrentRequest = CurrentRequestPair.Value;
			UPoolComponent** CurrentFinder = PoolComponentMap.Find(CurrentRequest.IndicatorType);

			int RequestShowerNum = CurrentRequest.Amount;
			int CurrentShowerNum = CurrentShowerArray.Num();

			if (RequestShowerNum != CurrentShowerNum && CurrentFinder != nullptr)
			{
				int numRemove = FMath::Max(RequestShowerNum - CurrentShowerNum, 0);
				int numAdd = FMath::Max(CurrentShowerNum - RequestShowerNum, 0);
				UPoolComponent* CurrentPool = *CurrentFinder;
				for (int i = 0; i < numRemove; ++i)
				{
					if (AActionIndicatorShowerBase* RemoveTarget = CurrentShowerArray.Pop()) CurrentPool->EnqueueInstance(RemoveTarget);
				}
				for (int i = 0; i < numAdd; i++)
				{
					TSoftObjectPtr<AActor> CurrentShower = CurrentPool->DequeueInstance();
					if (!CurrentShower.IsValid()) continue;
					AActionIndicatorShowerBase* NewShower = Cast<AActionIndicatorShowerBase>(CurrentShower.Get());
					CurrentShowerArray.AddUnique(NewShower);
				}
			}

			CurrentRequestNode->UpdateIndicatorArray(CurrentRequestPair.Key, CurrentClaim, Input, CurrentShowerArray, bIsIconPreview);
		}
	}
}

void UActionIndicatorBase::InitializePool()
{
	for (auto& CurrentPoolPair : PoolComponentMap)
	{
		UPoolComponent* CurrentPool = CurrentPoolPair.Value;
		if (!IsValid(CurrentPool)) continue;
		CurrentPool->EnqueueAll();
	}
	ShowerActiveMap.Empty();
}
