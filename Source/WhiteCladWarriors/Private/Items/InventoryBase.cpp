#include "Items/InventoryBase.h"
#include "Items/InventoryLayoutMath.h"
#include "Items/InventorySlot.h"
#include "Items/ItemBase.h"
#include "Items/ItemInstanceBase.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

// One game-thread edit; no listener observes a half-updated graph.
// Reentrant writes to this inventory fail until every notification has completed.
struct FInventoryEditScope
{
    TStrongObjectPtr<UInventoryBase> KeepAlive;
    EInventoryResult Result;
    bool bAcquired;
    explicit FInventoryEditScope(UInventoryBase* Inventory)
        : KeepAlive(IsInGameThread() ? Inventory : nullptr),
          Result(IsInGameThread() ? Inventory->BeginEdit() : EInventoryResult::WrongThread),
          bAcquired(Result == EInventoryResult::Success) {}
    ~FInventoryEditScope() { if (bAcquired) KeepAlive->FinishEdit(); }
    FInventoryEditScope(const FInventoryEditScope&) = delete;
    FInventoryEditScope& operator=(const FInventoryEditScope&) = delete;
};

// Reservation crosses inventory boundaries: user comparison/migration hooks cannot
// adopt the same detached input into a second inventory while its first edit is pending.
struct FInventoryItemReservation
{
    TStrongObjectPtr<UItemInstanceBase> Item;
    explicit FInventoryItemReservation(UItemInstanceBase* InItem, bool bAllowStateInitialization = false)
        : Item(InItem)
    {
        check(!Item->bReservedForInventoryOperation);
        Item->bReservedForInventoryOperation = true;
        Item->bAllowReservedStateInitialization = bAllowStateInitialization;
    }
    ~FInventoryItemReservation()
    {
        Item->bReservedForInventoryOperation = false;
        Item->bAllowReservedStateInitialization = false;
    }
    FInventoryItemReservation(const FInventoryItemReservation&) = delete;
    FInventoryItemReservation& operator=(const FInventoryItemReservation&) = delete;
};

UInventoryBase::UInventoryBase() { PrimaryComponentTick.bCanEverTick = false; }

EInventoryResult UInventoryBase::BeginEdit()
{
    if (!IsInGameThread()) return EInventoryResult::WrongThread;
    if (!IsValid(this)) return EInventoryResult::InvalidArgument;
    if (bEditing) return EInventoryResult::Busy;
    bEditing = true;
    PendingChanges = FInventoryChangeSet{};
    return EInventoryResult::Success;
}

void UInventoryBase::FinishEdit()
{
    for (FInventorySlotChange& Change : PendingChanges.Slots)
    {
        Change.NewItem = Change.Slot->GetItem();
        Change.NewPosition = Change.Slot->Position;
        Change.NewAmount = Change.Slot->AllocatedAmount;
    }
    PendingChanges.Slots.RemoveAll([](const FInventorySlotChange& C)
    {
        return C.OldItem == C.NewItem && C.OldPosition == C.NewPosition
            && C.OldAmount == C.NewAmount;
    });
    for (FInventoryItemChange& Change : PendingChanges.Items)
    {
        Change.NewCount = Change.Item->CurrentStack;
        Change.bIsOwned = Change.Item->InventoryOwner.Get() == this;
    }
    PendingChanges.Items.RemoveAll([](const FInventoryItemChange& C)
    { return C.OldCount == C.NewCount && C.bWasOwned == C.bIsOwned; });

#if DO_GUARD_SLOW
    check(ValidateInvariants());
#endif
    // Internal consistency NEVER relies on delegate binding order.
    if (PendingChanges.Slots.Num() || PendingChanges.Items.Num()
        || PendingChanges.Replacements.Num() || PendingChanges.bCapacityChanged)
    {
        OnChangedNative.Broadcast(PendingChanges);
        OnChanged.Broadcast(PendingChanges);
        for (const FInventoryItemChange& Change : PendingChanges.Items)
            if (Change.OldCount != Change.NewCount)
                Change.Item->OnStackChanged.Broadcast(Change.OldCount, Change.NewCount);
        for (const FInventoryInstanceReplacement& Change : PendingChanges.Replacements)
            Change.OldItem->OnReplaced.Broadcast(Change.NewItem);
    }
    PendingChanges = FInventoryChangeSet{};
    bEditing = false;
}

void UInventoryBase::TouchSlot(UInventorySlot* Slot)
{
    if (PendingChanges.Slots.ContainsByPredicate(
        [Slot](const FInventorySlotChange& C) { return C.Slot == Slot; })) return;
    FInventorySlotChange& Change = PendingChanges.Slots.AddDefaulted_GetRef();
    Change.Slot = Slot;
    Change.OldItem = Slot->GetItem();
    Change.OldPosition = Slot->Position;
    Change.OldAmount = Slot->AllocatedAmount;
}

void UInventoryBase::TouchItem(UItemInstanceBase* Item)
{
    if (PendingChanges.Items.ContainsByPredicate(
        [Item](const FInventoryItemChange& C) { return C.Item == Item; })) return;
    FInventoryItemChange& Change = PendingChanges.Items.AddDefaulted_GetRef();
    Change.Item = Item;
    Change.OldCount = Item->CurrentStack;
    Change.bWasOwned = Item->InventoryOwner.Get() == this;
}

bool UInventoryBase::Owns(const UItemInstanceBase* Item) const
{
    return IsValid(Item) && !Item->bRetired && Item->InventoryOwner.Get() == this;
}

bool UInventoryBase::OwnsSlot(const UInventorySlot* Slot) const
{
    return IsValid(Slot) && Slot->InventoryOwner.Get() == this
        && Owns(Slot->Item.Get()) && FindSlotAt(Slot->Position) == Slot;
}

bool UInventoryBase::InitializeCapacity(int32 InCapacity)
{
    if (!IsInGameThread() || bEditing || InCapacity < 0
        || ItemArray.Num() || IssuedSlots.Num()) return false;
    return InventorySucceeded(SetCapacity(InCapacity));
}

bool UInventoryBase::RegisterDefinition(UItemBase* Definition)
{
    if (!IsInGameThread() || bEditing || !IsValid(Definition)
        || !Definition->SealDefinition()) return false;
    Definitions.Add(Definition->GetItemId(), Definition);
    return true;
}

int64 UInventoryBase::GetCountById(FName ItemId) const
{ return TotalsById.FindRef(ItemId); }

void UInventoryBase::AddTotal(FName ItemId, int64 Delta)
{
    const int64 NewTotal = TotalsById.FindRef(ItemId) + Delta;
    check(NewTotal >= 0);
    if (NewTotal == 0) TotalsById.Remove(ItemId);
    else TotalsById.Add(ItemId, NewTotal);
}

TArray<UItemInstanceBase*> UInventoryBase::GetItemSnapshot() const
{
    TArray<UItemInstanceBase*> Result;
    Result.Reserve(ItemArray.Num());
    for (const auto& Item : ItemArray) Result.Add(Item.Get());
    return Result;
}

TArray<UInventorySlot*> UInventoryBase::GetSlotSnapshot() const
{
    TArray<UInventorySlot*> Result;
    Result.Reserve(IssuedSlots.Num());
    for (const auto& Slot : IssuedSlots) Result.Add(Slot.Get());
    return Result;
}

int32 UInventoryBase::LowerBoundSlot(int32 Position) const
{
    return InventoryLayout::LowerBound(IssuedSlots.Num(), Position,
        [this](int32 Index) { return IssuedSlots[Index]->Position; });
}

int32 UInventoryBase::FindSlotIndex(int32 Position) const
{
    const int32 Index = LowerBoundSlot(Position);
    return IssuedSlots.IsValidIndex(Index) && IssuedSlots[Index]->Position == Position
        ? Index : INDEX_NONE;
}

UInventorySlot* UInventoryBase::FindSlotAt(int32 Position) const
{
    const int32 Index = FindSlotIndex(Position);
    return Index == INDEX_NONE ? nullptr : IssuedSlots[Index].Get();
}

int32 UInventoryBase::ResolveFirstFreePosition()
{
    if (bFreePositionDirty)
    {
        FirstFreePosition = InventoryLayout::FirstGap(Capacity, IssuedSlots.Num(),
            [this](int32 Index) { return IssuedSlots[Index]->Position; });
        bFreePositionDirty = false;
    }
    // Handles editor-configured Capacity == 0 before any explicit initialization.
    return FirstFreePosition >= 0 && FirstFreePosition < Capacity
        ? FirstFreePosition : INDEX_NONE;
}

void UInventoryBase::InsertIntoItem(UItemInstanceBase* Item, UInventorySlot* Slot)
{
    const int32 Index = InventoryLayout::LowerBound(Item->AllocatedSlots.Num(), Slot->Position,
        [Item](int32 I) { return Item->AllocatedSlots[I]->GetPosition(); });
    Item->AllocatedSlots.Insert(TWeakObjectPtr<UInventorySlot>(Slot), Index);
}

void UInventoryBase::RemoveFromItem(UItemInstanceBase* Item, UInventorySlot* Slot)
{
    const int32 Index = InventoryLayout::LowerBound(Item->AllocatedSlots.Num(), Slot->Position,
        [Item](int32 I) { return Item->AllocatedSlots[I]->GetPosition(); });
    check(Item->AllocatedSlots.IsValidIndex(Index) && Item->AllocatedSlots[Index].Get() == Slot);
    Item->AllocatedSlots.RemoveAt(Index);
}

UInventorySlot* UInventoryBase::IssueSlot(UItemInstanceBase* Item, int32 Amount, int32 PreferredPosition)
{
    check(bEditing && Owns(Item));
    const int32 Position = PreferredPosition == INDEX_NONE ? ResolveFirstFreePosition() : PreferredPosition;
    check(Position >= 0 && Position < Capacity && !FindSlotAt(Position));

    UInventorySlot* Slot = NewObject<UInventorySlot>(this);
    TouchSlot(Slot); // Before = no allocation.
    Slot->InventoryOwner = this;
    Slot->Item = Item;
    Slot->Position = Position;
    Slot->AllocatedAmount = Amount;
    const int32 Index = LowerBoundSlot(Position);
    IssuedSlots.Insert(Slot, Index);
    InsertIntoItem(Item, Slot);

    // Advance from the just-issued hole; do not rescan the occupied prefix.
    if (!bFreePositionDirty && Position == FirstFreePosition)
    {
        int32 Next = Position + 1;
        for (int32 I = Index + 1; I < IssuedSlots.Num() && IssuedSlots[I]->Position == Next; ++I)
            ++Next;
        FirstFreePosition = Next < Capacity ? Next : INDEX_NONE;
    }
    return Slot;
}

void UInventoryBase::ReleaseSlot(UInventorySlot* Slot)
{
    TouchSlot(Slot);
    UItemInstanceBase* Item = Slot->Item.Get();
    const int32 Position = Slot->Position;
    const int32 Index = FindSlotIndex(Position);
    check(Index != INDEX_NONE && IssuedSlots[Index] == Slot);
    RemoveFromItem(Item, Slot);
    IssuedSlots.RemoveAt(Index);
    if (!bFreePositionDirty)
        FirstFreePosition = FirstFreePosition == INDEX_NONE
            ? Position : FMath::Min(FirstFreePosition, Position);
    Slot->InventoryOwner.Reset();
    Slot->Item.Reset();
    Slot->Position = INDEX_NONE;
    Slot->AllocatedAmount = 0;
}

void UInventoryBase::SetAllocation(UInventorySlot* Slot, int32 Amount)
{
    if (Slot->AllocatedAmount == Amount) return;
    TouchSlot(Slot);
    Slot->AllocatedAmount = Amount;
}

void UInventoryBase::AddAllocation(UItemInstanceBase* Item, int32 Amount)
{
    const int32 Max = Item->GetMaxStackEachSlot();
    // A drag can put a partial stack anywhere, so inspect THIS item's slots from the front.
    // No search over unrelated items or empty cells.
    for (const auto& WeakSlot : Item->AllocatedSlots)
    {
        UInventorySlot* Slot = WeakSlot.Get();
        const int32 Added = FMath::Min(Amount, Max - Slot->AllocatedAmount);
        if (Added > 0) SetAllocation(Slot, Slot->AllocatedAmount + Added);
        Amount -= Added;
        if (Amount == 0) return;
    }
    // Never mutate AllocatedSlots while range-iterating it above.
    while (Amount > 0)
    {
        const int32 Added = FMath::Min(Amount, Max);
        IssueSlot(Item, Added);
        Amount -= Added;
    }
}

void UInventoryBase::RemoveAllocation(UItemInstanceBase* Item, int32 Amount)
{
    while (Amount > 0)
    {
        UInventorySlot* Slot = Item->AllocatedSlots.Last().Get();
        const int32 Removed = FMath::Min(Amount, Slot->AllocatedAmount);
        const int32 Remaining = Slot->AllocatedAmount - Removed;
        if (Remaining == 0) ReleaseSlot(Slot);
        else SetAllocation(Slot, Remaining);
        Amount -= Removed;
    }
}

void UInventoryBase::RepackAllocation(UItemInstanceBase* Item)
{
    const int32 Total = Item->CurrentStack;
    const int32 Max = Item->GetMaxStackEachSlot();
    const int32 Required = InventoryLayout::RequiredSlots(Total, Max);
    check(Required >= 0);
    while (Item->AllocatedSlots.Num() > Required)
        ReleaseSlot(Item->AllocatedSlots.Last().Get());
    while (Item->AllocatedSlots.Num() < Required)
        IssueSlot(Item, 0); // Temporary, never visible to listeners.
    for (int32 I = 0; I < Required; ++I)
        SetAllocation(Item->AllocatedSlots[I].Get(), I + 1 == Required
            ? InventoryLayout::LastAmount(Total, Max) : Max);
}

UItemInstanceBase* UInventoryBase::FindCompatible(const UItemInstanceBase* Candidate,
    const UItemInstanceBase* Exclude) const
{
    for (const auto& Item : ItemArray)
        if (Item != Exclude && Item->CanStackWith(Candidate)) return Item.Get();
    return nullptr;
}

EInventoryResult UInventoryBase::CheckGrowth(const UItemInstanceBase* Existing,
    const UItemBase* Definition, int32 Amount) const
{
    if (Amount <= 0) return EInventoryResult::InvalidArgument;
    if (!IsValid(Definition) || Definition->GetItemId().IsNone()) return EInventoryResult::InvalidDefinition;
    const int32 Max = Definition->GetMaxStackEachSlot();
    const int32 InventoryMax = Definition->GetMaxStackEachInventory();
    if (Max <= 0 || InventoryMax < -1) return EInventoryResult::InvalidDefinition;
    const int64 NewCount = int64(Existing ? Existing->CurrentStack : 0) + Amount;
    if (NewCount > MAX_int32) return EInventoryResult::QuantityOverflow;
    if (InventoryMax >= 0 && GetCountById(Definition->GetItemId()) + Amount > InventoryMax)
        return EInventoryResult::InventoryLimitReached;
    // Aggregate free capacity is exact even if several assigned slots are partial.
    const int64 ExistingFree = Existing
        ? int64(Existing->AllocatedSlots.Num()) * Max - Existing->CurrentStack : 0;
    const int64 Missing = FMath::Max<int64>(0, int64(Amount) - ExistingFree);
    const int64 ExtraSlots = Missing / Max + (Missing % Max != 0);
    return ExtraSlots <= int64(Capacity) - IssuedSlots.Num()
        ? EInventoryResult::Success : EInventoryResult::CapacityExceeded;
}

void UInventoryBase::DiscardDetached(UItemInstanceBase* Item)
{
    check(!Item->InventoryOwner.IsValid() && Item->AllocatedSlots.Num() == 0);
    Item->CurrentStack = 0;
    Item->bRetired = true;
    if (Item->Base) Item->Base->UnregisterInstance(Item);
}

bool UInventoryBase::TryAddById(FName ItemId, int32 Amount, UItemInstanceBase*& ResultItem)
{ return InventorySucceeded(AddById(ItemId, Amount, ResultItem)); }

bool UInventoryBase::TryAddByBase(UItemBase* Definition, int32 Amount, UItemInstanceBase*& ResultItem)
{ return InventorySucceeded(AddByBase(Definition, Amount, ResultItem)); }

bool UInventoryBase::TryAddInstance(UItemInstanceBase* Incoming, UItemInstanceBase*& ResultItem)
{ return InventorySucceeded(AddInstance(Incoming, ResultItem)); }

bool UInventoryBase::TryAddToItem(UItemInstanceBase* Item, int32 Amount)
{ return InventorySucceeded(AddToItem(Item, Amount)); }

EInventoryResult UInventoryBase::AddById(FName ItemId, int32 Amount, UItemInstanceBase*& ResultItem)
{
    ResultItem = nullptr;
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (ItemId.IsNone() || Amount <= 0) return EInventoryResult::InvalidArgument;
    const TObjectPtr<UItemBase>* Found = Definitions.Find(ItemId);
    if (!Found) return EInventoryResult::UnknownItemId;
    if (!IsValid(Found->Get()) || (*Found)->GetItemId() != ItemId)
        return EInventoryResult::InvalidDefinition;
    return AddByBaseInEdit(Found->Get(), Amount, ResultItem);
}

EInventoryResult UInventoryBase::AddByBase(UItemBase* Definition, int32 Amount, UItemInstanceBase*& ResultItem)
{
    ResultItem = nullptr;
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    return AddByBaseInEdit(Definition, Amount, ResultItem);
}

EInventoryResult UInventoryBase::AddByBaseInEdit(UItemBase* Definition, int32 Amount,
    UItemInstanceBase*& ResultItem)
{
    check(bEditing);
    if (Amount <= 0) return EInventoryResult::InvalidArgument;
    if (!IsValid(Definition)) return EInventoryResult::InvalidDefinition;
    TStrongObjectPtr<UItemInstanceBase> Incoming(Definition->CreateItemInstance(Amount));
    if (!Incoming.IsValid()) return EInventoryResult::InvalidDefinition;
    const EInventoryResult Result = AddInstanceInEdit(Incoming.Get(), ResultItem);
    if (InventorySucceeded(Result)) return Result;
    // This was a speculative acquisition, not existing world stock.
    DiscardDetached(Incoming.Get());
    return Result;
}

EInventoryResult UInventoryBase::AddInstance(UItemInstanceBase* Incoming, UItemInstanceBase*& ResultItem)
{
    ResultItem = nullptr;
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    return AddInstanceInEdit(Incoming, ResultItem);
}

EInventoryResult UInventoryBase::AddInstanceInEdit(UItemInstanceBase* Incoming,
    UItemInstanceBase*& ResultItem)
{
    check(bEditing);
    if (!IsValid(Incoming)) return EInventoryResult::InvalidArgument;
    if (Incoming->bReservedForInventoryOperation) return EInventoryResult::Busy;
    if (Incoming->bRetired) return EInventoryResult::Retired;
    if (!Incoming->InventoryOwner.IsExplicitlyNull() || Incoming->AllocatedSlots.Num() != 0)
        return EInventoryResult::AlreadyOwned;
    if (Incoming->CurrentStack <= 0) return EInventoryResult::InvalidArgument;
    FInventoryItemReservation Reservation(Incoming);
    UItemInstanceBase* Existing = FindCompatible(Incoming);
    const int32 Amount = Incoming->CurrentStack;
    const EInventoryResult Growth = CheckGrowth(Existing, Incoming->Base, Amount);
    if (Growth != EInventoryResult::Success) return Growth;

    TouchItem(Incoming);
    UItemInstanceBase* Target = Existing ? Existing : Incoming;
    if (Existing)
    {
        TouchItem(Existing);
        Existing->CurrentStack += Amount;
    }
    else
    {
        ItemArray.Add(Incoming);
        Incoming->InventoryOwner = this;
    }
    AddAllocation(Target, Amount);
    AddTotal(Target->Base->GetItemId(), Amount);
    if (Existing) DiscardDetached(Incoming);
    ResultItem = Target;
    return EInventoryResult::Success;
}

EInventoryResult UInventoryBase::AddToItem(UItemInstanceBase* Item, int32 Amount)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!Owns(Item)) return EInventoryResult::NotOwned;
    const EInventoryResult Growth = CheckGrowth(Item, Item->Base, Amount);
    if (Growth != EInventoryResult::Success) return Growth;
    TouchItem(Item);
    Item->CurrentStack += Amount;
    AddAllocation(Item, Amount);
    AddTotal(Item->Base->GetItemId(), Amount);
    return EInventoryResult::Success;
}

void UInventoryBase::ApplyRemoval(UItemInstanceBase* Item, int32 Amount)
{
    TouchItem(Item);
    RemoveAllocation(Item, Amount);
    FinalizeRemoval(Item, Amount);
}

void UInventoryBase::FinalizeRemoval(UItemInstanceBase* Item, int32 Amount)
{
    TouchItem(Item);
    Item->CurrentStack -= Amount;
    AddTotal(Item->Base->GetItemId(), -int64(Amount));
    if (Item->CurrentStack == 0)
    {
        ItemArray.RemoveSingle(Item);
        Item->InventoryOwner.Reset();
        DiscardDetached(Item);
    }
}

bool UInventoryBase::TryRemoveItem(UItemInstanceBase* Item, int32 Amount)
{ return InventorySucceeded(RemoveItem(Item, Amount)); }

bool UInventoryBase::TryRemoveDefaultById(FName ItemId, int32 Amount)
{ return InventorySucceeded(RemoveDefaultById(ItemId, Amount)); }

bool UInventoryBase::CompactItem(UItemInstanceBase* Item)
{ return InventorySucceeded(NormalizeItem(Item)); }

EInventoryResult UInventoryBase::RemoveItem(UItemInstanceBase* Item, int32 Amount)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!Owns(Item)) return EInventoryResult::NotOwned;
    if (Amount <= 0) return EInventoryResult::InvalidArgument;
    if (Amount > Item->CurrentStack) return EInventoryResult::InsufficientQuantity;
    ApplyRemoval(Item, Amount);
    return EInventoryResult::Success;
}

EInventoryResult UInventoryBase::RemoveDefaultById(FName ItemId, int32 Amount)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (ItemId.IsNone() || Amount <= 0) return EInventoryResult::InvalidArgument;
    const TObjectPtr<UItemBase>* Found = Definitions.Find(ItemId);
    if (!Found) return EInventoryResult::UnknownItemId;
    if (!IsValid(Found->Get()) || (*Found)->GetItemId() != ItemId)
        return EInventoryResult::InvalidDefinition;
    TStrongObjectPtr<UItemInstanceBase> Probe((*Found)->CreateItemInstance(0));
    if (!Probe.IsValid()) return EInventoryResult::InvalidDefinition;
    FInventoryItemReservation Reservation(Probe.Get());
    UItemInstanceBase* Item = FindCompatible(Probe.Get());
    DiscardDetached(Probe.Get());
    if (!Item || Item->CurrentStack < Amount) return EInventoryResult::InsufficientQuantity;
    ApplyRemoval(Item, Amount);
    return EInventoryResult::Success;
}

EInventoryResult UInventoryBase::NormalizeItem(UItemInstanceBase* Item)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!Owns(Item)) return EInventoryResult::NotOwned;
    RepackAllocation(Item);
    return PendingChanges.Slots.IsEmpty() ? EInventoryResult::NoChange : EInventoryResult::Success;
}

bool UInventoryBase::TryMoveSlot(UInventorySlot* Slot, int32 NewPosition)
{
    FInventoryEditScope Edit(this);
    return Edit.bAcquired && InventorySucceeded(MoveSlotInEdit(Slot, NewPosition, true));
}

EInventoryResult UInventoryBase::MoveSlotToEmpty(UInventorySlot* Slot, int32 NewPosition)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    return MoveSlotInEdit(Slot, NewPosition, false);
}

EInventoryResult UInventoryBase::SwapSlots(UInventorySlot* A, UInventorySlot* B)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!OwnsSlot(A) || !OwnsSlot(B)) return EInventoryResult::NotOwned;
    return MoveSlotInEdit(A, B->Position, true);
}

EInventoryResult UInventoryBase::MoveSlotInEdit(UInventorySlot* Slot, int32 NewPosition, bool bSwapOccupied)
{
    check(bEditing);
    if (!OwnsSlot(Slot)) return EInventoryResult::NotOwned;
    if (NewPosition < 0 || NewPosition >= Capacity) return EInventoryResult::InvalidArgument;
    if (Slot->Position == NewPosition) return EInventoryResult::NoChange;

    const int32 FromIndex = FindSlotIndex(Slot->Position);
    const int32 ToIndex = FindSlotIndex(NewPosition);
    if (ToIndex != INDEX_NONE && !bSwapOccupied) return EInventoryResult::Occupied;
    check(FromIndex != INDEX_NONE && IssuedSlots[FromIndex] == Slot);
    UItemInstanceBase* Item = Slot->Item.Get();
    TouchSlot(Slot);

    if (ToIndex == INDEX_NONE)
    {
        RemoveFromItem(Item, Slot); // Remove before changing its sort key.
        IssuedSlots.RemoveAt(FromIndex);
        Slot->Position = NewPosition;
        IssuedSlots.Insert(Slot, LowerBoundSlot(NewPosition));
        InsertIntoItem(Item, Slot);
        bFreePositionDirty = true;
    }
    else
    {
        UInventorySlot* Other = IssuedSlots[ToIndex].Get();
        UItemInstanceBase* OtherItem = Other->Item.Get();
        TouchSlot(Other);
        if (Item == OtherItem)
        {
            // Both local positions are known before changing either sort key.
            const int32 LocalFrom = InventoryLayout::LowerBound(Item->AllocatedSlots.Num(),
                Slot->Position, [Item](int32 I) { return Item->AllocatedSlots[I]->GetPosition(); });
            const int32 LocalTo = InventoryLayout::LowerBound(Item->AllocatedSlots.Num(),
                Other->Position, [Item](int32 I) { return Item->AllocatedSlots[I]->GetPosition(); });
            Swap(Slot->Position, Other->Position);
            Swap(Item->AllocatedSlots[LocalFrom], Item->AllocatedSlots[LocalTo]);
        }
        else
        {
            RemoveFromItem(Item, Slot);
            RemoveFromItem(OtherItem, Other);
            Swap(Slot->Position, Other->Position);
            InsertIntoItem(Item, Slot);
            InsertIntoItem(OtherItem, Other);
        }
        // Occupied coordinates are unchanged: swap the two global array references.
        Swap(IssuedSlots[FromIndex], IssuedSlots[ToIndex]);
        // The existing first-free cache remains valid, or remains dirty as before.
    }
    return EInventoryResult::Success;
}

bool UInventoryBase::TryReplaceItem(UItemInstanceBase* OldItem, UItemBase* NewBase,
    UItemInstanceBase*& ResultItem)
{ return InventorySucceeded(ReplaceItem(OldItem, NewBase, ResultItem)); }

EInventoryResult UInventoryBase::ReplaceItem(UItemInstanceBase* OldItem, UItemBase* NewBase,
    UItemInstanceBase*& ResultItem)
{
    ResultItem = nullptr;
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!Owns(OldItem)) return EInventoryResult::NotOwned;
    if (!IsValid(NewBase)) return EInventoryResult::InvalidDefinition;
    TStrongObjectPtr<UItemInstanceBase> Candidate(NewBase->CreateItemInstance(0));
    if (!Candidate.IsValid()) return EInventoryResult::InvalidDefinition;
    FInventoryItemReservation Reservation(Candidate.Get(), true);
    const bool bStateCopied = OldItem->CopyStateTo(Candidate.Get());
    // Initialization permission is only for migration, never for state comparisons.
    Candidate->bAllowReservedStateInitialization = false;
    if (!bStateCopied)
    {
        DiscardDetached(Candidate.Get());
        return EInventoryResult::StateCopyRejected;
    }
    // The migration hook is not allowed to touch infrastructure.
    if (Candidate->Base != NewBase || Candidate->CurrentStack != 0 || Candidate->bRetired
        || !Candidate->InventoryOwner.IsExplicitlyNull() || Candidate->AllocatedSlots.Num())
    {
        ensureMsgf(false, TEXT("CopyStateTo modified inventory infrastructure."));
        return EInventoryResult::StateContractViolation;
    }

    UItemInstanceBase* Existing = FindCompatible(Candidate.Get(), OldItem);
    const int32 OldCount = OldItem->CurrentStack;
    const int64 Combined = int64(OldCount) + (Existing ? Existing->CurrentStack : 0);
    const FName OldId = OldItem->Base->GetItemId();
    const FName NewId = NewBase->GetItemId();
    const int32 NewMax = NewBase->GetMaxStackEachSlot();
    const int32 InventoryMax = NewBase->GetMaxStackEachInventory();
    const int64 NewFamilyTotal = GetCountById(NewId) + (OldId == NewId ? 0 : OldCount);
    const int64 AvailableSlots = int64(Capacity) - IssuedSlots.Num()
        + OldItem->AllocatedSlots.Num() + (Existing ? Existing->AllocatedSlots.Num() : 0);
    EInventoryResult Check = EInventoryResult::Success;
    if (Combined > MAX_int32) Check = EInventoryResult::QuantityOverflow;
    else if (NewMax <= 0 || InventoryMax < -1) Check = EInventoryResult::InvalidDefinition;
    else if (InventoryMax >= 0 && NewFamilyTotal > InventoryMax) Check = EInventoryResult::InventoryLimitReached;
    else if (Combined / NewMax + (Combined % NewMax != 0) > AvailableSlots) Check = EInventoryResult::CapacityExceeded;
    if (Check != EInventoryResult::Success)
    {
        DiscardDetached(Candidate.Get());
        return Check;
    }

    UItemInstanceBase* Target = Existing ? Existing : Candidate.Get();
    TouchItem(OldItem);
    TouchItem(Target);
    if (!Existing)
    {
        const int32 Index = ItemArray.IndexOfByKey(OldItem);
        check(Index != INDEX_NONE);
        ItemArray[Index] = Target;
        Target->InventoryOwner = this;
    }
    else ItemArray.RemoveSingle(OldItem);

    // Reuse allocation identities. Tooltip/widget clients can keep the same Slot pointer.
    for (const auto& WeakSlot : OldItem->AllocatedSlots)
    {
        UInventorySlot* Slot = WeakSlot.Get();
        TouchSlot(Slot);
        Slot->Item = Target;
        InsertIntoItem(Target, Slot);
    }
    OldItem->AllocatedSlots.Reset();
    OldItem->InventoryOwner.Reset();
    Target->CurrentStack = static_cast<int32>(Combined);
    DiscardDetached(OldItem);
    if (Existing) DiscardDetached(Candidate.Get());
    if (OldId != NewId)
    {
        AddTotal(OldId, -int64(OldCount));
        AddTotal(NewId, OldCount);
    }
    RepackAllocation(Target);

    FInventoryInstanceReplacement& Replacement = PendingChanges.Replacements.AddDefaulted_GetRef();
    Replacement.OldItem = OldItem;
    Replacement.NewItem = Target;
    ResultItem = Target;
    return EInventoryResult::Success;
}

EInventoryResult UInventoryBase::SplitSlot(UInventorySlot* Source, int32 Amount,
    int32 NewPosition, UInventorySlot*& ResultSlot)
{
    ResultSlot = nullptr;
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!OwnsSlot(Source)) return EInventoryResult::NotOwned;
    if (Amount <= 0 || Amount >= Source->AllocatedAmount || NewPosition < 0 || NewPosition >= Capacity)
        return EInventoryResult::InvalidArgument;
    if (FindSlotAt(NewPosition)) return EInventoryResult::Occupied;
    SetAllocation(Source, Source->AllocatedAmount - Amount);
    ResultSlot = IssueSlot(Source->Item.Get(), Amount, NewPosition);
    return EInventoryResult::Success;
}

EInventoryResult UInventoryBase::MergeSlots(UInventorySlot* Source, UInventorySlot* Target, int32 Amount)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!OwnsSlot(Source) || !OwnsSlot(Target)) return EInventoryResult::NotOwned;
    if (Source == Target || Amount <= 0) return EInventoryResult::InvalidArgument;
    // This is allocation merging, not semantic merging of two different instances.
    if (Source->Item != Target->Item) return EInventoryResult::IncompatibleItems;
    if (Amount > Source->AllocatedAmount) return EInventoryResult::InsufficientQuantity;
    if (Amount > Target->Item->GetMaxStackEachSlot() - Target->AllocatedAmount)
        return EInventoryResult::StackLimitReached;
    SetAllocation(Target, Target->AllocatedAmount + Amount);
    if (Amount == Source->AllocatedAmount) ReleaseSlot(Source);
    else SetAllocation(Source, Source->AllocatedAmount - Amount);
    return EInventoryResult::Success;
}

EInventoryResult UInventoryBase::RemoveFromSlot(UInventorySlot* Slot, int32 Amount)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (!OwnsSlot(Slot)) return EInventoryResult::NotOwned;
    if (Amount <= 0) return EInventoryResult::InvalidArgument;
    if (Amount > Slot->AllocatedAmount) return EInventoryResult::InsufficientQuantity;
    UItemInstanceBase* Item = Slot->Item.Get();
    if (Amount == Slot->AllocatedAmount) ReleaseSlot(Slot);
    else SetAllocation(Slot, Slot->AllocatedAmount - Amount);
    FinalizeRemoval(Item, Amount);
    return EInventoryResult::Success;
}

EInventoryResult UInventoryBase::SetCapacity(int32 NewCapacity)
{
    FInventoryEditScope Edit(this);
    if (!Edit.bAcquired) return Edit.Result;
    if (NewCapacity < 0) return EInventoryResult::InvalidArgument;
    if (NewCapacity == Capacity) return EInventoryResult::NoChange;
    if (!IssuedSlots.IsEmpty() && IssuedSlots.Last()->Position >= NewCapacity)
        return EInventoryResult::CapacityExceeded;
    PendingChanges.bCapacityChanged = true;
    PendingChanges.OldCapacity = Capacity;
    PendingChanges.NewCapacity = NewCapacity;
    Capacity = NewCapacity;
    bFreePositionDirty = true;
    return EInventoryResult::Success;
}

bool UInventoryBase::ValidateInvariants() const
{
    if (Capacity < 0 || IssuedSlots.Num() > Capacity) return false;
    for (const auto& Pair : Definitions)
        if (!IsValid(Pair.Value.Get()) || Pair.Key != Pair.Value->GetItemId()
            || !Pair.Value->IsDefinitionSealed()) return false;
    int32 PreviousPosition = INDEX_NONE;
    TSet<const UInventorySlot*> GlobalSlots;
    for (const auto& SlotPtr : IssuedSlots)
    {
        const UInventorySlot* Slot = SlotPtr.Get();
        if (!IsValid(Slot) || Slot->InventoryOwner.Get() != this
            || Slot->Position <= PreviousPosition || Slot->Position >= Capacity
            || Slot->AllocatedAmount <= 0 || GlobalSlots.Contains(Slot)) return false;
        PreviousPosition = Slot->Position;
        GlobalSlots.Add(Slot);
    }

    TMap<FName, int64> ComputedTotals;
    TSet<const UItemInstanceBase*> SeenItems;
    TSet<const UInventorySlot*> AssignedSlots;
    for (const auto& ItemPtr : ItemArray)
    {
        const UItemInstanceBase* Item = ItemPtr.Get();
        if (!Owns(Item) || !IsValid(Item->Base) || !Item->Base->IsDefinitionSealed() || Item->CurrentStack <= 0
            || Item->GetMaxStackEachSlot() <= 0 || SeenItems.Contains(Item)) return false;
        SeenItems.Add(Item);
        int64 Allocated = 0;
        PreviousPosition = INDEX_NONE;
        for (const auto& WeakSlot : Item->AllocatedSlots)
        {
            const UInventorySlot* Slot = WeakSlot.Get();
            if (!Slot || !GlobalSlots.Contains(Slot) || AssignedSlots.Contains(Slot)
                || Slot->Item.Get() != Item || Slot->Position <= PreviousPosition
                || Slot->AllocatedAmount > Item->GetMaxStackEachSlot()) return false;
            PreviousPosition = Slot->Position;
            Allocated += Slot->AllocatedAmount;
            AssignedSlots.Add(Slot);
        }
        if (Allocated != Item->CurrentStack) return false;
        ComputedTotals.FindOrAdd(Item->Base->GetItemId()) += Item->CurrentStack;
    }
    if (AssignedSlots.Num() != GlobalSlots.Num() || ComputedTotals.Num() != TotalsById.Num())
        return false;
    for (const auto& Pair : ComputedTotals)
        if (TotalsById.FindRef(Pair.Key) != Pair.Value) return false;
    if (!bFreePositionDirty)
    {
        const int32 Expected = InventoryLayout::FirstGap(Capacity, IssuedSlots.Num(),
            [this](int32 Index) { return IssuedSlots[Index]->Position; });
        const int32 Cached = FirstFreePosition >= 0 && FirstFreePosition < Capacity
            ? FirstFreePosition : INDEX_NONE;
        if (Expected != Cached) return false;
    }
    return true;
}
