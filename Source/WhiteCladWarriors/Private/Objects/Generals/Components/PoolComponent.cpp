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
	Result.SetNum(Count);
	for (int i = 0; i < Count; i++) Result[i] = SpawnInstance();
	return Result;
}

void UPoolComponent::OnInstanceDequeue_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (Target->GetClass()->ImplementsInterface(UPoolable::StaticClass())) IPoolable::Execute_OnPoolDequeue(Target, this);
	LiveArray.AddUnique(Target);
}

void UPoolComponent::OnInstanceEnqueue_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (Target->GetClass()->ImplementsInterface(UPoolable::StaticClass())) IPoolable::Execute_OnPoolEnqueue(Target, this);
	WaitQueue.Enqueue(Target);
	NumWait++;

	OnPoolStack.Broadcast();
	if(NumWait == 1) OnPoolStackFirst.Broadcast();
}

void UPoolComponent::Initialize_Implementation(TSubclassOf<AActor> WantTemplate, int WantCountOnStart)
{
	TemplateClass = WantTemplate;
	CountOnStart = WantCountOnStart;

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
			NumWait--;
			if (NumWait <= 0) OnPoolEmpty.Broadcast();
			return Result;
		}
	}
	Result = OnWaitQueueEmpty();
	return Result;
}

TArray<AActor*> UPoolComponent::DequeueAll_Implementation()
{
	TArray<AActor*> Result;
	if (WaitQueue.IsEmpty()) return Result;

	AActor* Current;
	while (WaitQueue.Dequeue(Current))
	{
		if (IsValid(Current))
		{
			OnInstanceDequeue(Current);
			Result.Add(Current);
		}
	}

	NumWait = 0;
	OnPoolEmpty.Broadcast();
	return Result;
}

void UPoolComponent::EnqueueInstance_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (LiveArray.Remove(Target)) OnInstanceEnqueue(Target);
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