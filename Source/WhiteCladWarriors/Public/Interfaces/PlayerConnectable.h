// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerConnectable.generated.h"

class AIngameController;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerConnectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IPlayerConnectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void OnPlayerConnected(AIngameController* NewPlayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void OnPlayerDisconnected(AIngameController* OldPlayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	AIngameController* GetConnectedPlayerController();
};
