// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Generals/Components/PoolComponent.h"
#include "Interfaces/Poolable.h"

void UPoolComponent::SpawnWait_Implementation(int Count)
{
	TArray<AActor*> SpawnResult = SpawnInstanceArray(Count);
	for (AActor* CurrentActor : SpawnResult) OnInstanceEnqueue(CurrentActor);
}

TArray<AActor*> UPoolComponent::SpawnLiveArray_Implementation(int Count)
{
	TArray<AActor*> SpawnResult = SpawnInstanceArray(Count);
	for (AActor* CurrentActor : SpawnResult) OnInstanceDequeue(CurrentActor);
	return SpawnResult;
}

AActor* UPoolComponent::SpawnLive_Implementation()
{
	AActor* Result = SpawnInstance();
	OnInstanceDequeue(Result);
	return Result;
}

AActor* UPoolComponent::SpawnInstance_Implementation()
{
	AActor* Result = nullptr;
	if (TemplateClass == nullptr) return Result;
	UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld)) return Result;
	Result = CurrentWorld->SpawnActor(TemplateClass);
	return Result;
}

TArray<AActor*> UPoolComponent::SpawnInstanceArray_Implementation(int Count)
{
	TArray<AActor*> Result;
	if (TemplateClass == nullptr || Count <= 0) return Result;
	UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld)) return Result;
	Result.SetNum(Count);
	for (int i = 0; i < Count; i++) Result[i] = CurrentWorld->SpawnActor(TemplateClass);
	return Result;
}

void UPoolComponent::OnInstanceDequeue_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	IPoolable::Execute_OnPoolDequeue(Target);
	LiveArray.AddUnique(Target);
}

void UPoolComponent::OnInstanceEnqueue_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	IPoolable::Execute_OnPoolEnqueue(Target);
	WaitQueue.Enqueue(Target);
}

AActor* UPoolComponent::OnWaitQueueEmpty_Implementation()
{
	SpawnWait(CountOnExpand);
	return SpawnLive();
}

void UPoolComponent::Initialize_Implementation(TSubclassOf<AActor> WantTemplate, int WantCountOnStart, int WantCountOnExpand)
{
	TemplateClass = WantTemplate;
	CountOnStart = WantCountOnStart;
	CountOnExpand = WantCountOnExpand;

	LiveArray.Reserve(CountOnStart);
	SpawnWait(CountOnStart);
}

AActor* UPoolComponent::DequeueInstance_Implementation()
{
	AActor* Result;

	while (WaitQueue.Dequeue(Result))
	{
		if (IsValid(Result))
		{
			OnInstanceDequeue(Result);
			return Result;
		}
	}

	Result = OnWaitQueueEmpty();
	return Result;
}

void UPoolComponent::EnqueueInstance_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (LiveArray.Remove(Target))
	{
		OnInstanceEnqueue(Target);
	}
}

void UPoolComponent::EnqueueAll_Implementation()
{
	if(LiveArray.IsEmpty()) return;
	for (AActor*& CurrentActor : LiveArray)
	{
		if (!IsValid(CurrentActor)) continue;
		if (LiveArray.Contains(CurrentActor)) OnInstanceEnqueue(CurrentActor);
	}
	LiveArray.Empty();
}