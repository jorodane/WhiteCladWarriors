// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/Stackable.h"
#include "ItemBase.generated.h"

/**
 * 
 */

class UItemInstanceBase;

DECLARE_DELEGATE_OneParam(FOnInstanceRemoved, UItemInstanceBase*, RemovedInstance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemInstanceReplaced, UItemInstanceBase*, ReplaceFrom, UItemInstanceBase*, ReplaceTo);

UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UItemBase : public UObject
{
	GENERATED_BODY()

protected:
	TArray<TWeakObjectPtr<UItemInstanceBase>> InstancedItems;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item")
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item")
	FText DisplayContext = FText::GetEmpty();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item")
	int MaxStackEachSlot = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item")
	int MaxStackEachInventory = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item")
	TSubclassOf<UItemInstanceBase> InstanceClass;

public:
	const FText& GetDisplayName() const { return DisplayName; }
	const FText& GetDisplayContext() const { return DisplayContext; }
	int GetMaxStackEachSlot() const { return MaxStackEachSlot; }
	int GetMaxStackEachInventory() const { return MaxStackEachInventory; }

public:
	UItemInstanceBase* CreateItemInstance();
	void RemoveItemInstance(UItemInstanceBase* TargetInstance);
	void ReplaceItemInstance(UItemInstanceBase* TargetInstance, UItemInstanceBase* NewInstance);
	void ReceiveItemRemoved(TObjectPtr<UItemInstanceBase> RemovedInstance);
	void ReceiveItemReplaced(TObjectPtr<UItemInstanceBase> ReplacedInstance, TObjectPtr<UItemInstanceBase> NewInstance);
};

UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UItemInstanceBase : public UObject, public IStackable
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable, Category = "Item")
	FOnItemInstanceReplaced OnReplaced;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TObjectPtr<UItemBase> Base = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Item")
	int CurrentStack = 0;

public:
	const UItemBase* GetBase() const { return Base; }
	void Replace(UItemInstanceBase* NewInstance);

public:
	virtual bool GetIsEmpty_Implementation() const;
	virtual int GetStack_Implementation() const; 
	virtual int GetEmptySpace_Implementation() const;
	virtual int GetMaxStackEachSlot_Implementation() const;
	virtual int GetMaxStackEachInventory_Implementation() const;
	virtual int SetStack_Implementation(int Amount);
	virtual int AddStack_Implementation(int Amount);
};

