// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UPoolComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Pool")
	TArray<AActor*> LiveArray;
	TQueue<AActor*> WaitQueue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ExposeOnSpawn = "true"))
	TSubclassOf<AActor> TemplateClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ExposeOnSpawn = "true"))
	int CountOnStart = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool", meta = (ExposeOnSpawn = "true"))
	int CountOnExpand = 5;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void SpawnWait(int Count);
	virtual void SpawnWait_Implementation(int Count);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	TArray<AActor*> SpawnLiveArray(int Count);
	virtual TArray<AActor*> SpawnLiveArray_Implementation(int Count);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	AActor* SpawnLive();
	virtual AActor* SpawnLive_Implementation();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	AActor* SpawnInstance();
	virtual AActor* SpawnInstance_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	TArray<AActor*> SpawnInstanceArray(int Count);
	virtual TArray<AActor*> SpawnInstanceArray_Implementation(int Count);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void OnInstanceDequeue(AActor* Target);
	void OnInstanceDequeue_Implementation(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void OnInstanceEnqueue(AActor* Target);
	void OnInstanceEnqueue_Implementation(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	AActor* OnWaitQueueEmpty();
	virtual AActor* OnWaitQueueEmpty_Implementation();

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void Initialize(TSubclassOf<AActor> WantTemplate, int WantCountOnStart = 10, int WantCountOnExpand = 5);
	virtual void Initialize_Implementation(TSubclassOf<AActor> WantTemplate, int WantCountOnStart = 10, int WantCountOnExpand = 5);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	AActor* DequeueInstance();
	virtual AActor* DequeueInstance_Implementation();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void EnqueueInstance(AActor* Target);
	virtual void EnqueueInstance_Implementation(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void EnqueueAll();
	void EnqueueAll_Implementation();
};
