// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define EXECUTOR_COUNT_ON_START 50
#define EXECUTOR_COUNT_ON_ADD 10

#include "Interfaces/MapSettingConnectable.h"
#include "MapInfo.h"
#include "Components/ActorComponent.h"
#include "ActionSetting.generated.h"

class AActionBase;
class AMapSetting;
class UActionExecutor;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UActionSetting : public UActorComponent, public IMapSettingConnectable
{
	GENERATED_BODY()

protected:
	static TObjectPtr<UActionSetting> CurrentSetting;
	TObjectPtr<AMapSetting> Owner;

	int64 lastExecutorID;

	UPROPERTY()
	TMap<int64, TObjectPtr<UActionExecutor>> ExecutorSpawned;

	UPROPERTY()
	TArray<TObjectPtr<UActionExecutor>> ExecutorWaitQueue;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TMap<FName, AActionBase*> ActionList;

public:
	void DestroyComponent(bool bPromoteChildren) override;

protected:
	void OnAttached_Implementation(AMapSetting* NewOwner);
	void OnDetached_Implementation(AMapSetting* OldOwner);

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void InitiateActions(AMapSetting* WantInfo);
	virtual void InitiateActions_Implementation(AMapSetting* WantInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	AActionBase* FindMissingAction(const FName& MissingActionName);
	virtual AActionBase* FindMissingAction_Implementation(const FName& MissingActionName);

protected:
	bool GetExecutor(int64 ID, TWeakObjectPtr<UActionExecutor>& Result);
	int64 ActivateExecutorFromPool(TWeakObjectPtr<UActionExecutor>& Result);
	void DeactivateExecutorToPool(int64 ID);
	void ExecutorReady(int amount);
	TObjectPtr<UActionExecutor> ExecutorReady();


public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	static AActionBase* GetAction(FName WantName);

	static bool ClaimGetExecutor(int64 ID, TWeakObjectPtr<UActionExecutor>& Result);
	static int64 ClaimActivateExecutorFromPool(TWeakObjectPtr<UActionExecutor>& Result);
	static void ClaimDeactivateExecutorToPool(int64 ID);
		
};
