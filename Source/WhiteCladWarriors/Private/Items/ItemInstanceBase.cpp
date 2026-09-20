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
    int MaxStack = Base->GetMaxStackEachSlot();
    //Amount = PushToExistSlots(Amount, MaxStack);
    if (Amount <= 0) return 0;
    //Amount = PushToClaimSlots(Amount, MaxStack);
    return Amount;
    UInventoryBase* Owner = GetInventory();
    if (IsValid(Owner))
    {
        int OriginStack = Stack;
        int Addable = 0;//Owner->GetAddableAmount(this, Amount);
        if (Addable <= 0) return Amount;
        Stack += Addable;
        OnStackChanged.Broadcast(OriginStack, Stack);
        //Owner->Notify_AddItem(this, Addable);
        return Amount - Addable;
    }
    else
    {
        Stack += Amount;
        return 0;
    }
}

int UItemInstanceBase::DecreaseStack(int Amount)
{
    int OriginStack = Stack;
    int Removable = FMath::Min(Stack, Amount);
    if (Removable <= 0) return Amount;
    Stack -= Removable;
    OnStackChanged.Broadcast(OriginStack, Stack);
    //UInventoryBase* Owner = GetInventory();
    //if (IsValid(Owner)) Owner->Notify_RemoveItem(this, Removable);
    return Amount - Removable;
}

//int UItemInstanceBase::PushToExistSlots(int Amount, int MaxStack)
//{
//
//}
//
//int UItemInstanceBase::PushToClaimSlots(int Amount, int MaxStack)
//{
//
//}
//
//int UItemInstanceBase::PopFromExistSlots(int Amount)
//{
//
//}

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
