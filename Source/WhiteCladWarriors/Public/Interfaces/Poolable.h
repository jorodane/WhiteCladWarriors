// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Poolable.generated.h"

class UPoolComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IPoolable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pool")
	void OnPoolEnqueue(UPoolComponent* EnqueueTo);
	virtual void OnPoolEnqueue_Implementation(UPoolComponent* EnqueueTo) { }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pool")
	void OnPoolDequeue(UPoolComponent* DequeueFrom);
	virtual void OnPoolDequeue_Implementation(UPoolComponent* DequeueFrom) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pool")
	bool ClaimPoolEnqueue();
	virtual bool ClaimPoolEnqueue_Implementation() { return false; }
};
