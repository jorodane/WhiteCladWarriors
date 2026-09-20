#include "Items/ItemBase.h"
#include "Items/ItemInstanceBase.h"
#include "Items/InventoryBase.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

UItemBase::UItemBase() { InstanceClass = UItemInstanceBase::StaticClass(); }

bool UItemBase::InitializeDefinition(FName InItemId, int32 InSlotMax,
    int32 InInventoryMax, TSubclassOf<UItemInstanceBase> InInstanceClass)
{
    if (!IsInGameThread() || bDefinitionSealed || InItemId.IsNone() || InSlotMax <= 0
        || InInventoryMax < -1 || !InInstanceClass) return false;
    //for (const auto& Weak : InstancedItems)
    //    if (const UItemInstanceBase* Item = Weak.Get())
    //        if (!Item->IsRetired()) return false;
    //ItemId = InItemId;
    //MaxStackEachSlot = InSlotMax;
    //MaxStackEachInventory = InInventoryMax;
    //InstanceClass = InInstanceClass;
    return true;
}

bool UItemBase::SealDefinition()
{
    //if (!IsInGameThread() || ItemId.IsNone() || MaxStackEachSlot <= 0
    //    || MaxStackEachInventory < -1 || !InstanceClass
    //    || InstanceClass->HasAnyClassFlags(CLASS_Abstract)) return false;
    bDefinitionSealed = true;
    return true;
}

UItemInstanceBase* UItemBase::CreateItemInstance(int32 Amount)
{
    if (Amount < 0 || !SealDefinition()) return nullptr;

    // Outer is not the logical inventory owner. That relation is explicit.
    UItemInstanceBase* Item = NewObject<UItemInstanceBase>(GetTransientPackage(), InstanceClass);
    //Item->Base = this;
    //Item->CurrentStack = Amount;
    InstancedItems.Add(Item);
    return Item;
}

void UItemBase::UnregisterInstance(UItemInstanceBase* Instance)
{
    InstancedItems.RemoveAll([Instance](const TWeakObjectPtr<UItemInstanceBase>& Weak)
    { return !Weak.IsValid() || Weak.Get() == Instance; });
}

TArray<UItemInstanceBase*> UItemBase::GetLiveInstances()
{
    //InstancedItems.RemoveAll([](const TWeakObjectPtr<UItemInstanceBase>& Weak)
    //{ return !Weak.IsValid() || Weak->IsRetired(); });
    TArray<UItemInstanceBase*> Result;
    Result.Reserve(InstancedItems.Num());
    for (const auto& Weak : InstancedItems) Result.Add(Weak.Get());
    return Result;
}

int64 UItemBase::GetOwnedTotal() const
{
    int64 Total = 0;
    //for (const auto& Weak : InstancedItems)
    //    if (const UItemInstanceBase* Item = Weak.Get())
    //        if (!Item->IsRetired() && IsValid(Item->GetInventoryOwner()))
    //            Total += Item->GetStack();
    return Total;
}
