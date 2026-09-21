#include "Items/ItemInstanceBase.h"
#include "Items/ItemBase.h"
#include "Items/InventoryBase.h"
#include "Items/InventorySlot.h"

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
    UInventoryBase* Owner = GetInventory();
    if (IsValid(Owner))
    {
        int OriginStack = Stack;
        int Addable = 0;//Owner->GetAddableAmount(this, Amount);
        if (Addable <= 0) return Amount;
        Stack += Addable;
        OnStackChanged.Broadcast(OriginStack, Stack);
        return Amount - Addable;
    }
    else
    {
        Stack += Amount;
        return 0;
    }
}

int UItemInstanceBase::PopFromExistSlots(int Amount)
{
    if (!Slots.IsEmpty())
    {
        for (int i = Slots.Num() - 1; i >= 0; --i)
        {
            if (Amount <= 0) break;
            TWeakObjectPtr<UInventorySlot> CurrentPtr = Slots[i];
            if (!CurrentPtr.IsValid()) continue;
            UInventorySlot* CurrentSlot = CurrentPtr.Get();
            bool bIsEmpty = false;
            Amount = CurrentSlot->AddAmount(Amount, bIsEmpty);
        }
    }

    return Amount;
}

void UItemInstanceBase::AddSlot(TObjectPtr<UInventorySlot> AddedSlot)
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

void UItemInstanceBase::RemoveSlot(TObjectPtr<UInventorySlot> RemovedSlot)
{
    Slots.Remove(RemovedSlot);
}

bool UItemInstanceBase::GetIsSameItem(const UItemInstanceBase* Other) const
{
    if (!IsValid(Other)) return false;
    if (Base == nullptr || Base != Other->Base) return false;
    return GetClass() == Other->GetClass();
}
