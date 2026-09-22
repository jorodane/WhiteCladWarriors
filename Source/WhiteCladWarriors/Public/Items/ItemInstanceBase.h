#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemInstanceBase.generated.h"

class UItemBase;
class UInventoryBase;
class UInventorySlot;
class UItemInstanceBase;
struct FInventoryItemReservation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemInstanceReplaced, UItemInstanceBase*, ReplaceTo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemStackChanged, int, OldCount, int, NewCount);

UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UItemInstanceBase : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Item")
    FOnItemInstanceReplaced OnReplaced;

    UPROPERTY(BlueprintAssignable, Category = "Item")
    FOnItemStackChanged OnStackChanged;

private:
    TArray<TWeakObjectPtr<UInventorySlot>> Slots;

    UPROPERTY(BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<UInventoryBase> InventoryFrom;

    UPROPERTY(BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UItemBase> Base = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    int Stack = 0;

public:
    int ClaimSlot(int Number);
    void FreeSlot(UInventorySlot* TargetSlot);
    void FreeSlot(TArray<UInventorySlot*> TargetSlots);

protected:
    int PushToExistSlots(int Amount, const int& MaxStack);
    int PushToClaimSlots(int Amount, const int& MaxStack);
    int PopFromExistSlots(int Amount);

    void OnSlotAdded(TObjectPtr<UInventorySlot> AddedSlot);
    void OnSlotRemoved(TObjectPtr<UInventorySlot> RemovedSlot);

public:
    UFUNCTION(BlueprintCallable, Category = "Item")
    int SetStack(int Amount);

    UFUNCTION(BlueprintCallable, Category="Item")
    int IncreaseStack(int Amount);

    UFUNCTION(BlueprintCallable, Category="Item")
    int DecreaseStack(int Amount);

    UFUNCTION(BlueprintPure, Category = "Item")
    UItemBase* GetBase() const { return Base; }

    UFUNCTION(BlueprintPure, Category = "Item")
    int GetStack() const { return Stack; }

    UFUNCTION(BlueprintPure, Category = "Item")
    bool GetIsEmpty() const { return Stack <= 0; }

    UFUNCTION(BlueprintPure, Category = "Item")
    UInventoryBase* GetInventory() const { return InventoryFrom.Get(); }

    UFUNCTION(BlueprintPure, Category = "Item")
    bool GetIsSameItem(const UItemInstanceBase* Other) const;

    //UFUNCTION(BlueprintPure, Category = "Item")
    //int GetMaxStackEachSlot() const { return Base ? Base->GetMaxStackEachSlot() : 0; }

    const TArray<TWeakObjectPtr<UInventorySlot>>& GetSlots() const { return Slots; }
};
