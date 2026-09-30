#include "Items/ItemInstanceBase.h"
#include "Items/ItemBase.h"
#include "Items/InventoryBase.h"
#include "Items/InventorySlot.h"

TArray<UInventorySlot*> UItemInstanceBase::ClaimAllocateSlot(int Number)
{
    if (Number <= 0 || !InventoryFrom.IsValid()) return TArray<UInventorySlot*>();
    return InventoryFrom->ClaimAllocateSlot(this, Number);
}

void UItemInstanceBase::ClaimFreeSlot(TObjectPtr<UInventorySlot> TargetSlot)
{
    if (!IsValid(TargetSlot)) return;
    if (!InventoryFrom.IsValid()) return;
    InventoryFrom->FreeSlot(TargetSlot);
    Slots.Remove(TargetSlot);
}

void UItemInstanceBase::ClaimFreeSlot(TArray<UInventorySlot*> TargetSlots)
{
    if (TargetSlots.IsEmpty()) return;
    if (!InventoryFrom.IsValid()) return;
    InventoryFrom->FreeSlot(TargetSlots);
    for (UInventorySlot* CurrentSlot : TargetSlots) Slots.Remove(CurrentSlot);
}

int UItemInstanceBase::PushToExistSlots(int Amount, const int& MaxStack)
{
    if (!Slots.IsEmpty())
    {
        for (int i = 0; i < Slots.Num(); ++i)
        {
            if (Amount <= 0) break;
            TWeakObjectPtr<UInventorySlot> CurrentPtr = Slots[i];
            if (!CurrentPtr.IsValid()) continue;
            UInventorySlot* CurrentSlot = CurrentPtr.Get();
            Amount = CurrentSlot->AddAmount(Amount, MaxStack);
        }
    }

    return Amount;
}

int UItemInstanceBase::PushToClaimSlots(int Amount, const int& MaxStack)
{
    if (Amount <= 0 || MaxStack <= 0) return 0;
    UInventoryBase* Owner = GetInventory();
    if (IsValid(Owner))
    {
        int NumClaimedSlot = FMath::CeilToInt((float)Amount / MaxStack);
        TArray<UInventorySlot*> ReceivedSlots = ClaimAllocateSlot(NumClaimedSlot);
        if (ReceivedSlots.IsEmpty()) return Amount;
        int NumReceivedSlot = ReceivedSlots.Num();
        int Addable;
        if (NumClaimedSlot <= NumReceivedSlot)
        {
            int LastReceivedSlotIndex = NumReceivedSlot - 1;
            int FullyAdded = MaxStack * LastReceivedSlotIndex;
            for (int i = 0; i < LastReceivedSlotIndex; i++) ReceivedSlots[i]->SetAmount(MaxStack);
            ReceivedSlots[LastReceivedSlotIndex]->SetAmount(Amount - FullyAdded);
            Addable = Amount;
        }
        else if(NumReceivedSlot > 0)
        {
            for (int i = 0; i < NumReceivedSlot; i++) ReceivedSlots[i]->SetAmount(MaxStack);
            Addable = NumReceivedSlot * MaxStack;
        }
        if (Addable <= 0) return Amount;

        return Amount - Addable;
    }

    return Amount;
}

int UItemInstanceBase::PopFromExistSlots(int Amount)
{
    if (!Slots.IsEmpty())
    {
        TArray<UInventorySlot*> RemovedSlot;
        for (int i = Slots.Num() - 1; i >= 0; --i)
        {
            if (Amount <= 0) break;
            TWeakObjectPtr<UInventorySlot> CurrentPtr = Slots[i];
            if (!CurrentPtr.IsValid()) continue;
            UInventorySlot* CurrentSlot = CurrentPtr.Get();
            bool bIsEmpty = false;
            Amount = CurrentSlot->RemoveAmount(Amount, bIsEmpty);
            if (bIsEmpty) RemovedSlot.Add(CurrentSlot);
        }
        ClaimFreeSlot(RemovedSlot);
    }

    return Amount;
}

void UItemInstanceBase::OnSlotAdded(TObjectPtr<UInventorySlot> AddedSlot)
{
    if (!IsValid(AddedSlot)) return;
    if (Slots.Find(AddedSlot) != INDEX_NONE) return;
    int NewPosition = AddedSlot->GetPosition();
    int NewIndex = Slots.IndexOfByPredicate([NewPosition](TWeakObjectPtr<UInventorySlot> CurrentPointer)->bool
        {
            UInventorySlot* FoundSlot = CurrentPointer.Get();
            if (!IsValid(FoundSlot)) return false;
            return FoundSlot->GetPosition() > NewPosition;
        });
    if (NewIndex == INDEX_NONE) Slots.Add(AddedSlot);
    else Slots.Insert(AddedSlot, NewIndex);
}

void UItemInstanceBase::OnSlotShifted(TObjectPtr<UInventorySlot> ShiftedSlot)
{
    if (!IsValid(ShiftedSlot)) return;

    int NumSlot = Slots.Num();
    if (NumSlot <= 0)
    {
        Slots.Add(ShiftedSlot);
        return;
    }
    int LastIndex = NumSlot - 1;
    int OriginIndex = Slots.Find(ShiftedSlot);
    if (OriginIndex == INDEX_NONE)
    {
        OnSlotAdded(ShiftedSlot);
        return;
    }

    int NewIndex = INDEX_NONE;
    if (OriginIndex > 0)
    {
        for (int i = OriginIndex - 1; i >= 0; --i)
        {
            int CompareResult = ShiftedSlot->ComparePosition(Slots[i]);
            if (CompareResult >= 0)
            {
                int CalculatedIndex = i + 1;
                if (CalculatedIndex < OriginIndex) NewIndex = CalculatedIndex;
                break;
            }
        }
    }

    if (NewIndex != INDEX_NONE && OriginIndex < LastIndex)
    {
        for (int i = OriginIndex + 1; i < NumSlot; ++i)
        {
            int CompareResult = ShiftedSlot->ComparePosition(Slots[i]);
            if (CompareResult < 0)
            {
                int CalculatedIndex = i - 1;
                if (CalculatedIndex > OriginIndex) NewIndex = CalculatedIndex;
                break;
            }
        }
    }

    if (NewIndex == INDEX_NONE) return;
    Slots.RemoveAt(OriginIndex);
    if (NewIndex >= LastIndex) Slots.Add(ShiftedSlot);
    else Slots.Insert(ShiftedSlot, NewIndex);
}


void UItemInstanceBase::OnSlotRemoved(TObjectPtr<UInventorySlot> RemovedSlot)
{
    Slots.Remove(RemovedSlot);
}


int UItemInstanceBase::SetStack(int Amount)
{
    if (Amount > Stack) IncreaseStack(Amount - Stack);
    else if (Amount < Stack) DecreaseStack(Stack - Amount);
    return Stack;
}

int UItemInstanceBase::IncreaseStack(int Amount)
{
    if (!IsValid(Base)) return Amount;
    int MaxStack        = Base->GetMaxStackEachSlot();
    int Left            = PushToExistSlots(Amount, MaxStack);
    if (Left > 0) Left  = PushToClaimSlots(Left, MaxStack);
    int Added = Amount - Left;
    Stack += Added;
    return Left;
}

int UItemInstanceBase::DecreaseStack(int Amount)
{
    int OriginStack = Stack;
    int Left = PopFromExistSlots(Amount);
    int Removed = Amount - Left;
    Stack -= Removed;
    OnStackChanged.Broadcast(OriginStack, Stack);
    return Left;
}

bool UItemInstanceBase::GetIsSameItem(const UItemInstanceBase* Other) const
{
    if (!IsValid(Other)) return false;
    if (Base == nullptr || Base != Other->Base) return false;
    return GetClass() == Other->GetClass();
}
