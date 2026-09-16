#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemInstanceBase.generated.h"

class UItemBase;
class UInventoryBase;
class UInventorySlot;
class UItemInstanceBase;
struct FInventoryItemReservation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnItemInstanceReplaced, UItemInstanceBase*, ReplaceTo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnItemStackChanged, int32, OldCount, int32, NewCount);

UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UItemInstanceBase : public UObject
{
    GENERATED_BODY()
public:
    // Public intentionally: C++ AddDynamic and Blueprint binding are both supported.
    // Project contract: only the inventory's commit path calls Broadcast.
    UPROPERTY(BlueprintAssignable, Category="Item")
    FOnItemInstanceReplaced OnReplaced;
    UPROPERTY(BlueprintAssignable, Category="Item")
    FOnItemStackChanged OnStackChanged;

    UFUNCTION(BlueprintPure, Category="Item")
    UItemBase* GetBase() const { return Base; }
    UFUNCTION(BlueprintPure, Category="Item")
    int32 GetStack() const { return CurrentStack; }
    UFUNCTION(BlueprintPure, Category="Item")
    bool GetIsEmpty() const { return CurrentStack == 0; }
    UFUNCTION(BlueprintPure, Category="Item")
    bool IsRetired() const { return bRetired; }
    // Use this in subclass initialization APIs. Expired owners are not detached owners.
    UFUNCTION(BlueprintPure, Category="Item")
    bool CanInitializeState() const;
    UFUNCTION(BlueprintPure, Category="Item")
    UInventoryBase* GetInventoryOwner() const;
    UFUNCTION(BlueprintPure, Category="Item")
    int32 GetMaxStackEachSlot() const;
    UFUNCTION(BlueprintPure, Category="Item")
    int32 GetMaxStackEachInventory() const;
    UFUNCTION(BlueprintPure, Category="Item")
    TArray<UInventorySlot*> GetSlotSnapshot() const;

    const TArray<TWeakObjectPtr<UInventorySlot>>& GetAllocatedSlots() const
    { return AllocatedSlots; }

    // Owned quantity changes always go through the allocation authority.
    UFUNCTION(BlueprintCallable, Category="Item")
    bool TryAddStack(int32 Amount);
    UFUNCTION(BlueprintCallable, Category="Item")
    bool TryRemoveStack(int32 Amount);

    bool CanStackWith(const UItemInstanceBase* Other) const;

    // Pure comparison contract: no mutation, no allocation, no notifications.
    // Base implementation only merges exact UItemInstanceBase objects.
    // Every state-bearing subclass must explicitly define state equivalence.
    UFUNCTION(BlueprintNativeEvent, Category="Item")
    bool HasSameStackState(const UItemInstanceBase* Other) const;
    virtual bool HasSameStackState_Implementation(const UItemInstanceBase* Other) const;

    // Copy semantic state ONLY into a fresh, detached candidate.
    // Never copy Owner, Slots, Count, delegates, or runtime identity.
    // False aborts replacement without changing the owned item.
    UFUNCTION(BlueprintNativeEvent, Category="Item")
    bool CopyStateTo(UItemInstanceBase* Destination) const;
    virtual bool CopyStateTo_Implementation(UItemInstanceBase* Destination) const;

private:
    UPROPERTY(Transient, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UItemBase> Base = nullptr;
    UPROPERTY(Transient, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
    int32 CurrentStack = 0;

private:
    friend class UItemBase;
    friend class UInventoryBase;
    friend struct FInventoryItemReservation;
    bool bReservedForInventoryOperation = false;
    bool bAllowReservedStateInitialization = false;
    UPROPERTY(Transient)
    TWeakObjectPtr<UInventoryBase> InventoryOwner;
    // Non-owning, position-sorted reverse index. Allocator owns the slot objects.
    UPROPERTY(Transient)
    TArray<TWeakObjectPtr<UInventorySlot>> AllocatedSlots;
    UPROPERTY(Transient)
    bool bRetired = false;
};
