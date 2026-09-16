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
public:
    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetPosition() const { return Position; }
    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetAllocatedAmount() const { return AllocatedAmount; }
    UFUNCTION(BlueprintPure, Category="Inventory")
    UInventoryBase* GetInventoryOwner() const;
    UFUNCTION(BlueprintPure, Category="Inventory")
    UItemInstanceBase* GetItem() const;

private:
    friend class UInventoryBase;
    UPROPERTY(Transient)
    TWeakObjectPtr<UInventoryBase> InventoryOwner;
    UPROPERTY(Transient)
    TWeakObjectPtr<UItemInstanceBase> Item;
    UPROPERTY(Transient)
    int32 Position = INDEX_NONE;
    UPROPERTY(Transient)
    int32 AllocatedAmount = 0;
};
