// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/ActionSetting.h"
#include "Settings/MapSetting.h"
#include "Actions/Executables/ActionExecutor.h"

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

bool UActionSetting::GetExecutor(uint64 ID, TObjectPtr<UActionExecutor>& Result)
{
	TObjectPtr<UActionExecutor>* Founded = ExecutorSpawned.Find(ID);
	if (Founded) Result = *Founded;
	return IsValid(Result);
}

uint64 UActionSetting::ActivateExecutorFromPool()
{
	uint64 ResultID = lastExecutorID++;

	TObjectPtr<UActionExecutor> ResultExecutor = ExecutorWaitQueue.Pop(false);
	if (!ResultExecutor)
	{
		ExecutorReady(EXECUTOR_COUNT_ON_ADD);
		ResultExecutor = ExecutorReady();
		if (!ResultExecutor) return -1;
	}

	ResultExecutor->ExecutorID = ResultID;
	ExecutorSpawned.Add(ResultID, ResultExecutor);

	return ResultID;
}

void UActionSetting::DeactivateExecutorToPool(uint64 ID)
{
	TObjectPtr<UActionExecutor> ResultExecutor;
	if (ExecutorSpawned.RemoveAndCopyValue(ID, ResultExecutor))
	{
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
	}
	return nullptr;
}

bool UActionSetting::ClaimGetExecutor(uint64 ID, TWeakObjectPtr<UActionExecutor>& Result)
{
	if (CurrentSetting)
	{
		return CurrentSetting->ClaimGetExecutor(ID, Result);
	}
	return false;
}

uint64 UActionSetting::ClaimActivateExecutorFromPool()
{
	if (CurrentSetting)
	{
		return CurrentSetting->ActivateExecutorFromPool();
	}
	return -1;
}

void UActionSetting::ClaimDeactivateExecutorToPool(uint64 ID)
{
	if (CurrentSetting)
	{
		CurrentSetting->DeactivateExecutorToPool(ID);
	}
}
