#include "Items/ItemInstanceBase.h"
#include "Items/ItemBase.h"
#include "Items/InventoryBase.h"
#include "Items/InventorySlot.h"

UInventoryBase* UItemInstanceBase::GetInventoryOwner() const { return InventoryOwner.Get(); }
int32 UItemInstanceBase::GetMaxStackEachSlot() const
{ return Base ? Base->GetMaxStackEachSlot() : 0; }
int32 UItemInstanceBase::GetMaxStackEachInventory() const
{ return Base ? Base->GetMaxStackEachInventory() : 0; }

bool UItemInstanceBase::CanInitializeState() const
{
    return !bRetired && (!bReservedForInventoryOperation || bAllowReservedStateInitialization)
        && InventoryOwner.IsExplicitlyNull() && AllocatedSlots.IsEmpty();
}

TArray<UInventorySlot*> UItemInstanceBase::GetSlotSnapshot() const
{
    TArray<UInventorySlot*> Result;
    Result.Reserve(AllocatedSlots.Num());
    for (const auto& WeakSlot : AllocatedSlots)
        if (UInventorySlot* Slot = WeakSlot.Get()) Result.Add(Slot);
    return Result;
}

bool UItemInstanceBase::TryAddStack(int32 Amount)
{
    UInventoryBase* Owner = InventoryOwner.Get();
    return Owner && Owner->TryAddToItem(this, Amount);
}

bool UItemInstanceBase::TryRemoveStack(int32 Amount)
{
    UInventoryBase* Owner = InventoryOwner.Get();
    return Owner && Owner->TryRemoveItem(this, Amount);
}

bool UItemInstanceBase::CanStackWith(const UItemInstanceBase* Other) const
{
    return IsValid(Other) && !bRetired && !Other->bRetired
        && Base && Base == Other->Base && GetClass() == Other->GetClass()
        && HasSameStackState(Other) && Other->HasSameStackState(this);
}

bool UItemInstanceBase::HasSameStackState_Implementation(const UItemInstanceBase* Other) const
{
    // Unknown derived state must not accidentally disappear through merging.
    return Other && GetClass() == StaticClass() && Other->GetClass() == StaticClass();
}

bool UItemInstanceBase::CopyStateTo_Implementation(UItemInstanceBase* Destination) const
{
    return Destination && GetClass() == StaticClass() && Destination->GetClass() == StaticClass();
}
