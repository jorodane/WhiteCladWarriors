#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventorySlot.generated.h"

class UInventoryBase;
class UItemInstanceBase;

// An issued allocation, NOT a permanently existing grid cell and NOT a widget.
UCLASS(BlueprintType)
class WHITECLADWARRIORS_API UInventorySlot : public UObject
{
    GENERATED_BODY()

protected:

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    TWeakObjectPtr<UItemInstanceBase> Item;

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    TWeakObjectPtr<UInventoryBase> Inventory;

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    int Position = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    int Amount = 0;

public:
    UFUNCTION(BlueprintPure, Category="Item")
    UItemInstanceBase* GetItem() const { return Item.Get(); }

    UFUNCTION(BlueprintPure, Category = "Item")
    UInventoryBase* GetInventory() const { return Inventory.Get(); }

    UFUNCTION(BlueprintPure, Category = "Item")
    int GetPosition() const { return Position; }

    UFUNCTION(BlueprintPure, Category = "Item")
    int GetAmount() const { return Amount; }

    UFUNCTION(BlueprintCallable, Category = "Item")
    UItemInstanceBase* SetItem(UItemInstanceBase* NewItemInstance) { return Item.Get(); }

    UFUNCTION(BlueprintCallable, Category = "Item")
    UInventoryBase* SetInventory(UInventoryBase* NewInventory) { return Inventory.Get(); }

    UFUNCTION(BlueprintCallable, Category = "Item")
    int SetPosition(int NewPosition) { return Position = NewPosition; }

    UFUNCTION(BlueprintCallable, Category = "Item")
    int SetAmount(int Value) { return Amount = Value; }

    //UFUNCTION(BlueprintPure, Category = "Item")
    //int AddAmount(int Value, int MaxAmount);

    //UFUNCTION(BlueprintPure, Category = "Item")
    //int RemoveAmount(int Value, bool& OutIsEmpty);
};
