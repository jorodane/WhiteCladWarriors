// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/ActionSetting.h"
#include "Settings/MapSetting.h"
#include "Actions/Executables/ActionExecutor.h"
#include "Actions/ActionBase.h"

TObjectPtr<UActionSetting> UActionSetting::CurrentSetting = nullptr;

void UActionSetting::DestroyComponent(bool bPromoteChildren)
{
	if (Owner) IMapSettingConnectable::Execute_OnDetached(this, Owner);
	Super::DestroyComponent(bPromoteChildren);
}

void UActionSetting::OnAttached_Implementation(AMapSetting* NewOwner)
{
	Owner = NewOwner;
	CurrentSetting = this;
	InitiateActions(NewOwner);
}
void UActionSetting::OnDetached_Implementation(AMapSetting* OldOwner)
{
	if (OldOwner == Owner) Owner = nullptr;
	if (CurrentSetting == this)
	{
		CurrentSetting = nullptr;
	}
}

void UActionSetting::InitiateActions_Implementation(AMapSetting* WantInfo)
{
	ExecutorReady(EXECUTOR_COUNT_ON_START);
}

AActionBase* UActionSetting::FindMissingAction_Implementation(const FName& MissingActionName)
{
	return nullptr;
}

bool UActionSetting::GetExecutor(int64 ID, TWeakObjectPtr<UActionExecutor>& Result)
{
	TObjectPtr<UActionExecutor>* Founded = ExecutorSpawned.Find(ID);
	if (Founded) Result = *Founded;
	return Result.IsValid();
}

int64 UActionSetting::ActivateExecutorFromPool(TWeakObjectPtr<UActionExecutor>& Result)
{
	int64 ResultID = lastExecutorID++;

	TObjectPtr<UActionExecutor> ResultExecutor = ExecutorWaitQueue.Pop(false);
	if (!ResultExecutor)
	{
		ExecutorReady(EXECUTOR_COUNT_ON_ADD);
		ResultExecutor = ExecutorReady();
		if (!ResultExecutor) return -1;
	}

	ResultExecutor->ExecutorID = ResultID;
	ExecutorSpawned.Add(ResultID, ResultExecutor);

	Result = ResultExecutor;
	return ResultID;
}

void UActionSetting::DeactivateExecutorToPool(int64 ID)
{
	TObjectPtr<UActionExecutor> ResultExecutor;
	if (ExecutorSpawned.RemoveAndCopyValue(ID, ResultExecutor))
	{
		ResultExecutor->Clear();
		ExecutorWaitQueue.Add(ResultExecutor);
	}
}

void UActionSetting::ExecutorReady(int amount)
{
	for (int i = 0; i < amount; i++)
	{
		ExecutorWaitQueue.Add(ExecutorReady());
	}
}
TObjectPtr<UActionExecutor> UActionSetting::ExecutorReady()
{
	return NewObject<UActionExecutor>(this);
}


AActionBase* UActionSetting::GetAction(FName WantName)
{
	if (CurrentSetting)
	{
		if (AActionBase** Result = CurrentSetting->ActionList.Find(WantName))
		{
			return *Result;
		}
		else
		{
			AActionBase* CreatedAction = CurrentSetting->FindMissingAction(WantName);
			if (IsValid(CreatedAction))
			{
				CurrentSetting->ActionList.Add(WantName, CreatedAction);
				return CreatedAction;
			}
		}
	}
	return nullptr;
}

bool UActionSetting::ClaimGetExecutor(int64 ID, TWeakObjectPtr<UActionExecutor>& Result)
{
	if (CurrentSetting)
	{
		return CurrentSetting->GetExecutor(ID, Result);
	}
	return false;
}

int64 UActionSetting::ClaimActivateExecutorFromPool(TWeakObjectPtr<UActionExecutor>& Result)
{
	if (CurrentSetting)
	{
		return CurrentSetting->ActivateExecutorFromPool(Result);
	}
	return -1;
}

void UActionSetting::ClaimDeactivateExecutorToPool(int64 ID)
{
	if (CurrentSetting)
	{
		CurrentSetting->DeactivateExecutorToPool(ID);
	}
}
