// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Units/UnitBase.h"
#include "Generals/ReservedActionMessage.h"
#include "Actions/UnitActionComponent.h"
#include "Actions/ActionExecutor.h"
#include "Actions/ActionBase.h"
#include "Settings/ActionSetting.h"

void FMainActionInfo::Clear(const UActionExecutor* OldExecutor) 
{ 
	if (!Executor.IsValid() || Executor.Get() == OldExecutor) Clear();
}

void FMainActionInfo::SetActionMessage_Simple(FName Message)
{
	if (IsValid()) Executor.Get()->SetActionMessage_Simple(Component.Get(), Message);
}

bool FMainActionInfo::Cancel(bool bWantStopMovement)
{
	if (bIsCancelable)
	{
		End(bWantStopMovement);
		return true;
	}
	else return false;
}

void FMainActionInfo::End(bool bWantStopMovement)
{
	if (IsValid())
	{
		Component.Get()->OnEndMainAction(Executor.Get(), bWantStopMovement);
		Clear();
	}
}


bool FMainActionInfo::IsValid() const
{
	return Executor.IsValid() && Component.IsValid();
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
	
	for (UActorComponent* CurrentComponent : GetComponents()) AddActionComponent(Cast<UUnitActionComponent>(CurrentComponent));
}

TArray<AActionBase*> AUnitBase::GetActionList() const
{
	TArray<AActionBase*> Result;
	ActionMap.GetKeys(Result);
	return Result;
}


bool AUnitBase::GetSimpleAction(const FInputPackage& CurrentInput, AActionBase*& OutAction, TArray<UUnitActionComponent*>& OutComponents) const
{
	int MaxOrder = 0;
	AActionBase* MaxAction = nullptr;
	for (const TPair<AActionBase*, TArray<UUnitActionComponent*>>& CurrentPair : ActionMap)
	{
		AActionBase* CurrentAction = CurrentPair.Key;
		if (!IsValid(CurrentAction)) continue;
		for (UUnitActionComponent* CurrentComponent : CurrentPair.Value)
		{
			if (!IsValid(CurrentComponent)) continue;

			int CurrentOrder = CurrentAction->GetSimpleActionOrder(CurrentInput, CurrentComponent);

			if (CurrentOrder > MaxOrder)
			{
				MaxOrder = CurrentOrder;
				MaxAction = CurrentAction;
			}
		}
	}
	if (MaxAction == nullptr) return false;

	OutAction = MaxAction;
	const TArray<UUnitActionComponent*>* ResultComponents = ActionMap.Find(OutAction);
	OutComponents = *ResultComponents;
	return MaxOrder > 0;
}

void AUnitBase::AddActionComponent(UUnitActionComponent* NewComponent)
{
	if (!IsValid(NewComponent)) return;

	ActionComponentArray.AddUnique(NewComponent);

	for (FName CurrentName : NewComponent->ActionList)
	{
		if (AActionBase* CurrentAction = UActionSetting::GetAction(CurrentName))
		{
			TArray<UUnitActionComponent*>& CurrentList = ActionMap.FindOrAdd(CurrentAction);
			CurrentList.Add(NewComponent);
		}
	}
};


bool AUnitBase::SetMainAction(const FMainActionInfo& Info)
{
	if(Info.IsValid()) return SetMainAction(Info.Executor.Get(), Info.Component.Get(), Info.bIsCancelable);
	else return SetMainAction(nullptr, nullptr);
}

bool AUnitBase::GetMainActionCancelable_Implementation() const 
{ return MainAction.bIsCancelable; };

bool AUnitBase::SetMainAction(UActionExecutor* Executor, UUnitActionComponent* Component, bool bIsCancelable, bool bIsStopMovement)
{
	bool Result = MainAction.Cancel(bIsStopMovement);
	if (Result) MainAction.Set(Executor, Component, bIsCancelable);
	return Result;
}

void AUnitBase::EndMainAction(bool bIsStopMovement)
{
	if (MainAction.IsValid()) MainAction.End(bIsStopMovement);
}

void AUnitBase::EndMainAction(UActionExecutor* OldExecutor, bool bIsStopMovement)
{
	MainAction.End(bIsStopMovement);
}


void AUnitBase::Die_Implementation() 
{ 
	for (UActorComponent* CurrentComponent : GetComponents())
	{
		if (UUnitComponentBase* AsUnitComponent = Cast<UUnitComponentBase>(CurrentComponent)) AsUnitComponent->BroadcastRemoveMessage();
	}
	OnUnitDie.Broadcast(this); 
}
