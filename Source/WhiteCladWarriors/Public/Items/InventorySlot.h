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
    int Position = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    int Index = INDEX_NONE;

public:
    UFUNCTION(BlueprintPure, Category="Item")
    UItemInstanceBase* GetItem() const { return Item.Get(); }

    UFUNCTION(BlueprintPure, Category = "Item")
    UInventoryBase* GetInventory() const;

    UFUNCTION(BlueprintPure, Category = "Item")
    int GetPosition() const { return Position; }
    UFUNCTION(BlueprintPure, Category = "Item")
    int GetIndex() const { return Index; }
    UFUNCTION(BlueprintPure, Category = "Item")
    int GetAmount() const;
};
