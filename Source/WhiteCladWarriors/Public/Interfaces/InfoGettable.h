// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoGettable.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInfoGettable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IInfoGettable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Info")
	TSubclassOf<UUserWidget> GetTargetInfoClass();
	virtual TSubclassOf<UUserWidget> GetTargetInfoClass_Implementation() { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Info")
	void OnInfoConnected(UUserWidget* NewWidget);
	virtual void OnInfoConnected_Implementation(UUserWidget* NewWidget) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Info")
	void OnInfoDisconnected(UUserWidget* OldWidget);
	virtual void OnInfoDisconnected_Implementation(UUserWidget* OldWidget) {}
};
