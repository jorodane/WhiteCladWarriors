// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Stackable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStackable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IStackable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stack")
	bool GetIsEmpty() const;
	virtual bool GetIsEmpty_Implementation() const { return 0; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stack")
	int GetStack() const;
	virtual int GetStack_Implementation() const { return 0; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stack")
	int GetEmptySpace() const;
	virtual int GetEmptySpace_Implementation() const { return 0; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stack")
	int GetMaxStackEachSlot() const;
	virtual int GetMaxStackEachSlot_Implementation() const { return 0; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stack")
	int GetMaxStackEachInventory() const;
	virtual int GetMaxStackEachInventory_Implementation() const { return 0; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stack")
	int SetStack(int Amount);
	virtual int SetStack_Implementation(int Amount) { return Amount; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stack")
	int AddStack(int Amount);
	virtual int AddStack_Implementation(int Amount) { return Amount; }
};
