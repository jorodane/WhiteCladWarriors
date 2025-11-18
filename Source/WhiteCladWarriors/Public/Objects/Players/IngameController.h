// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IngameController.generated.h"

/**
 * 
 */
class AUnitBase;
class AOperator;

UCLASS()
class WHITECLADWARRIORS_API AIngameController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* aPawn) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<AOperator> ConnectedOperator;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<AUnitBase> ConnectedHero;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnOperatorConnected(AOperator* NewOperator);
	void OnOperatorConnected_Implementation(AOperator* NewOperator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnOperatorDisconnected(AOperator* OldOperator);
	void OnOperatorDisconnected_Implementation(AOperator* OldOperator);
};
