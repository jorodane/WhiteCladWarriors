#include "Items/InventorySlot.h"
#include "Items/ItemInstanceBase.h"

UInventoryBase* UInventorySlot::GetInventory() const
{ 
	UItemInstanceBase* ItemGetter = GetItem();
	if (IsValid(ItemGetter)) return ItemGetter->GetInventory();
	return nullptr;
}

int UInventorySlot::GetAmount() const
{
	UItemInstanceBase* ItemGetter = GetItem();
	if (IsValid(ItemGetter)) return ItemGetter->GetAmount(Index);
	return 0;
}

