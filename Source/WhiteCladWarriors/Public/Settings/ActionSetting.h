// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interfaces/MapSettingConnectable.h"
#include "MapInfo.h"
#include "Components/ActorComponent.h"
#include "ActionSetting.generated.h"

class AActionBase;
class AMapSetting;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UActionSetting : public UActorComponent, public IMapSettingConnectable
{
	GENERATED_BODY()

protected:
	static TObjectPtr<UActionSetting> CurrentSetting;
	TObjectPtr<AMapSetting> Owner;

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
	void InitiateActions(const FMapInfo& WantInfo);
	virtual void InitiateActions_Implementation(const FMapInfo& WantInfo);

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	static AActionBase* GetAction(FName WantName);
//public:	
//	// Sets default values for this component's properties
//	UActionSetting();
//
//protected:
//	// Called when the game starts
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
