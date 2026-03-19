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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ExposeOnSpawn = "true"))
	TSubclassOf<AActor> Template;

	TQueue<AActor*> WaitQueue;
	TSet<AActor*> Created;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ExposeOnSpawn = "true"))
	int CountOnStart = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool", meta = (ExposeOnSpawn = "true"))
	int CountOnExpand = 5;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void SpawnWait(int Count);
	virtual void SpawnWait_Implementation(int Count);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	TArray<AActor*> SpawnInstance(int Count);
	virtual TArray<AActor*> SpawnInstance_Implementation(int Count);

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	AActor* CreateInstance();
	virtual AActor* CreateInstance_Implementation();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pool")
	void DestroyInstance(AActor* Target);
	virtual void DestroyInstance_Implementation(AActor* Target);
};
