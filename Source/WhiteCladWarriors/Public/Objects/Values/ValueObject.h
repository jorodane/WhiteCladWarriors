// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ValueObject.generated.h"
/**
 * 
 */
class UValueContainer;

UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UValueObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Container")
	UValueContainer* ConnectedContainer = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	void ContainerConnect(UValueContainer* NewContainer);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void ContainerDisconnect();

	UFUNCTION(BlueprintImplementableEvent, Category = "Container")
	void OnContainerConnected(UValueContainer* NewContainer);
	UFUNCTION(BlueprintImplementableEvent, Category = "Container")
	void OnContainerDisconnected(UValueContainer* OldContainer);
};
