// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IngameController.generated.h"

/**
 * 
 */
class UUnitMainComponent;
class UHeroMainComponent;
class AOperator;

UCLASS()
class WHITECLADWARRIORS_API AIngameController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	FText UserName;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<AOperator> ConnectedOperator;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<UHeroMainComponent> ConnectedHero;


protected:
	virtual void OnPossess(APawn* aPawn) override;

public:
	UFUNCTION(BlueprintPure, Category = "Player")
	FVector GetTerrainPosition(float HeightOffset);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void SetCursor(EMouseCursor::Type NewCursor);
	virtual void SetCursor_Implementation(EMouseCursor::Type NewCursor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnOperatorConnected(AOperator* NewOperator);
	virtual void OnOperatorConnected_Implementation(AOperator* NewOperator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player")
	void OnOperatorDisconnected(AOperator* OldOperator);
	virtual void OnOperatorDisconnected_Implementation(AOperator* OldOperator);
};
