#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/InventoryTypes.h"
#include "InventoryBase.generated.h"

class UItemBase;
class UItemInstanceBase;
class UInventorySlot;
struct FInventoryEditScope;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WHITECLADWARRIORS_API UInventoryBase : public UActorComponent
{
    GENERATED_BODY()
public:
    UInventoryBase();

    UPROPERTY(BlueprintAssignable, Category="Inventory")
    FOnInventoryChanged OnChanged;

    FOnInventoryChangedNative OnChangedNative;

    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetCapacity() const { return Capacity; }
    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetFreeCellCount() const { return Capacity - IssuedSlots.Num(); }
    UFUNCTION(BlueprintPure, Category="Inventory")
    int64 GetCountById(FName ItemId) const;
    UFUNCTION(BlueprintPure, Category="Inventory")
    UInventorySlot* FindSlotAt(int32 Position) const;
    UFUNCTION(BlueprintPure, Category="Inventory")
    TArray<UItemInstanceBase*> GetItemSnapshot() const;
    UFUNCTION(BlueprintPure, Category="Inventory")
    TArray<UInventorySlot*> GetSlotSnapshot() const;

    const TArray<TObjectPtr<UItemInstanceBase>>& GetItems() const { return ItemArray; }
    const TArray<TObjectPtr<UInventorySlot>>& GetIssuedSlots() const { return IssuedSlots; }

    // Only on an empty inventory. Resizing an occupied inventory needs a policy.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool InitializeCapacity(int32 InCapacity);
    // Changes future ID resolution only. Existing items are NOT migrated here.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool RegisterDefinition(UItemBase* Definition);

    // All-or-nothing additions. Failure does not modify inventory or Incoming.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryAddById(FName ItemId, int32 Amount, UItemInstanceBase*& ResultItem);
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryAddByBase(UItemBase* Definition, int32 Amount, UItemInstanceBase*& ResultItem);
    // Takes a detached positive-count instance, or merges it into a compatible one.
    // On success ResultItem is the survivor. Merge does NOT broadcast OnReplaced.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryAddInstance(UItemInstanceBase* Incoming, UItemInstanceBase*& ResultItem);
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryAddToItem(UItemInstanceBase* Item, int32 Amount);
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryRemoveItem(UItemInstanceBase* Item, int32 Amount);
    // Only a state matching a freshly created default instance is eligible.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryRemoveDefaultById(FName ItemId, int32 Amount);

    // Moves the allocation object. Occupied destination means a positional swap.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryMoveSlot(UInventorySlot* Slot, int32 NewPosition);
    // Repack ONE item's quantities. Does not globally sort items or grid positions.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool CompactItem(UItemInstanceBase* Item);

    // New class, explicit semantic migration, capacity preflight, then atomic commit.
    // Existing slots are reused; only excess/insufficient allocations are changed.
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool TryReplaceItem(UItemInstanceBase* OldItem, UItemBase* NewBase,
        UItemInstanceBase*& ResultItem);

    // Detailed commands. Legacy Try* bool functions above forward to these contracts.
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult AddById(FName ItemId, int32 Amount, UItemInstanceBase*& ResultItem);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult AddByBase(UItemBase* Definition, int32 Amount, UItemInstanceBase*& ResultItem);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult AddInstance(UItemInstanceBase* Incoming, UItemInstanceBase*& ResultItem);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult AddToItem(UItemInstanceBase* Item, int32 Amount);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult RemoveItem(UItemInstanceBase* Item, int32 Amount);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult RemoveDefaultById(FName ItemId, int32 Amount);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult NormalizeItem(UItemInstanceBase* Item);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult ReplaceItem(UItemInstanceBase* OldItem, UItemBase* NewBase,
        UItemInstanceBase*& ResultItem);

    // These preserve item identity and never choose a drop-on-occupied policy implicitly.
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult MoveSlotToEmpty(UInventorySlot* Slot, int32 NewPosition);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult SwapSlots(UInventorySlot* A, UInventorySlot* B);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult SplitSlot(UInventorySlot* Source, int32 Amount, int32 NewPosition,
        UInventorySlot*& ResultSlot);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult MergeSlots(UInventorySlot* Source, UInventorySlot* Target, int32 Amount);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult RemoveFromSlot(UInventorySlot* Slot, int32 Amount);
    UFUNCTION(BlueprintCallable, Category="Inventory|Commands")
    EInventoryResult SetCapacity(int32 NewCapacity);

    UFUNCTION(BlueprintCallable, Category="Inventory|Debug")
    bool ValidateInvariants() const;

private:
    friend struct FInventoryEditScope;

    UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(ClampMin="0"))
    int32 Capacity = 30;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UItemInstanceBase>> ItemArray;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UInventorySlot>> IssuedSlots;
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UItemBase>> Definitions;

    // Derived query index. Mutated only together with ItemArray / CurrentStack.
    TMap<FName, int64> TotalsById;
    int32 FirstFreePosition = 0;
    bool bFreePositionDirty = false;
    bool bEditing = false;

    // Reflected ownership keeps old/new objects alive through all callbacks.
    UPROPERTY(Transient)
    FInventoryChangeSet PendingChanges;

    bool Owns(const UItemInstanceBase* Item) const;
    bool OwnsSlot(const UInventorySlot* Slot) const;
    EInventoryResult BeginEdit();
    void FinishEdit();
    void TouchSlot(UInventorySlot* Slot);
    void TouchItem(UItemInstanceBase* Item);
    void AddTotal(FName ItemId, int64 Delta);
    void DiscardDetached(UItemInstanceBase* Item);
    UItemInstanceBase* FindCompatible(const UItemInstanceBase* Candidate,
        const UItemInstanceBase* Exclude = nullptr) const;
    EInventoryResult CheckGrowth(const UItemInstanceBase* Existing, const UItemBase* Definition,
        int32 Amount) const;
    EInventoryResult AddByBaseInEdit(UItemBase* Definition, int32 Amount,
        UItemInstanceBase*& ResultItem);
    EInventoryResult AddInstanceInEdit(UItemInstanceBase* Incoming,
        UItemInstanceBase*& ResultItem);
    EInventoryResult MoveSlotInEdit(UInventorySlot* Slot, int32 NewPosition, bool bSwapOccupied);

    int32 LowerBoundSlot(int32 Position) const;
    int32 FindSlotIndex(int32 Position) const;
    int32 ResolveFirstFreePosition();
    void InsertIntoItem(UItemInstanceBase* Item, UInventorySlot* Slot);
    void RemoveFromItem(UItemInstanceBase* Item, UInventorySlot* Slot);
    UInventorySlot* IssueSlot(UItemInstanceBase* Item, int32 Amount, int32 PreferredPosition = INDEX_NONE);
    void ReleaseSlot(UInventorySlot* Slot);
    void SetAllocation(UInventorySlot* Slot, int32 Amount);
    void AddAllocation(UItemInstanceBase* Item, int32 Amount);
    void RemoveAllocation(UItemInstanceBase* Item, int32 Amount);
    void RepackAllocation(UItemInstanceBase* Item);
    void ApplyRemoval(UItemInstanceBase* Item, int32 Amount);
    void FinalizeRemoval(UItemInstanceBase* Item, int32 Amount);
};
