// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Settings/MapComponentBase.h"
#include "ActionSetting.generated.h"

class AActionBase;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UActionSetting : public UMapComponentBase
{
	GENERATED_BODY()

protected:
	static TObjectPtr<UActionSetting> CurrentSetting;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	TMap<FName, AActionBase*> ActionList;

protected:
	void OnAttached(AMapSetting* Owner) override;
	void OnDetached(AMapSetting* Owner) override;

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
