// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionIndicatorBase.h"
#include "Actions/ActionIndicatorShowerBase.h"
#include "Actions/ActionBehaviorNode.h"
#include "Objects/Players/Operator.h"

void UActionIndicatorBase::BeginPlay()
{
	Super::BeginPlay();

	for (const auto& CurrentShowerPair : ShowerDefaultClassMap)
	{
		EInputIndicatorType CurrentType = CurrentShowerPair.Key;
		TSubclassOf<AActionIndicatorShowerBase> CurrentClass = CurrentShowerPair.Value;
		ShowerPrefare(CurrentType, CurrentClass, ShowerPoolDefaultCount);
	}
}

AActionIndicatorShowerBase* UActionIndicatorBase::ShowerPop(EInputIndicatorType WantType)
{
	AActionIndicatorShowerBase* Result = nullptr;
	
	TArray<AActionIndicatorShowerBase*>* Queue = ShowerPoolMap.Find(WantType);

	if (Queue)
	{
		Result = Queue->Pop();
		if (!IsValid(Result))
		{
			ShowerPrefareWithoutTemplate(WantType, ShowerPoolAdditionalCount);
			Result = Queue->Pop();
		}
	}
	else
	{
		ShowerPrefareWithoutTemplate(WantType, ShowerPoolAdditionalCount);
		Queue = ShowerPoolMap.Find(WantType);
		Result = Queue->Pop();
	}

	Result->OnPoolDequeue();

	return Result;
}

void UActionIndicatorBase::ShowerPush(EInputIndicatorType WantType, AActionIndicatorShowerBase* WantShower)
{
	if (!IsValid(WantShower)) return;
	auto& TargetQueue = ShowerPoolMap.FindOrAdd(WantType);
	TargetQueue.AddUnique(WantShower);
	WantShower->OnPoolEnqueue();
}

void UActionIndicatorBase::ShowerPushFromSet(EInputIndicatorType WantType, AActionIndicatorShowerBase* WantShower, UActionBehaviorNode* TargetNode)
{
	if (!IsValid(TargetNode)) return;
	if (auto CurrentSet = ShowerActiveMap.Find(TargetNode)) CurrentSet->Remove(WantShower);
	ShowerPush(WantType, WantShower);
}

void UActionIndicatorBase::ShowerPrefareWithoutTemplate(EInputIndicatorType WantType, int Count)
{
	if (TSubclassOf<AActionIndicatorShowerBase>* Template = ShowerDefaultClassMap.Find(WantType))
	{
		ShowerPrefare(WantType, *Template, Count);
	}
}

void UActionIndicatorBase::ShowerPrefare(EInputIndicatorType WantType, TSubclassOf<AActionIndicatorShowerBase> WantClass, int Count)
{
	if (!IsValid(WantClass)) return;
	UWorld* CurrentWorld = GetWorld();
	auto& CurrentQueue = ShowerPoolMap.FindOrAdd(WantType);
	for (int i = 0; i < Count; i++) ShowerCreateAndPush(CurrentWorld, WantClass, CurrentQueue);
}

AActionIndicatorShowerBase* UActionIndicatorBase::ShowerCreateAndPush(UWorld* WorldContext, TSubclassOf<AActionIndicatorShowerBase> Template, TArray<AActionIndicatorShowerBase*>& Pool)
{
	AActionIndicatorShowerBase* Result = nullptr;
	if (!IsValid(WorldContext)) return Result;

	Result = Cast<AActionIndicatorShowerBase>(WorldContext->SpawnActor(Template));

	if (!IsValid(Result)) return Result;

	Pool.AddUnique(Result);
	Result->OnPoolEnqueue();
	return Result;
}

void UActionIndicatorBase::ShowerClear()
{
	for (const auto& CurrentPair : ShowerActiveMap)
	{
		for (AActionIndicatorShowerBase* CurrentShower : CurrentPair.Value)
		{
			if (!IsValid(CurrentShower)) continue;
			EInputIndicatorType CurrentType = CurrentShower->GetIndicatorType();
			ShowerPush(CurrentType, CurrentShower);
		}
	}
	ShowerActiveMap.Empty();
}

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