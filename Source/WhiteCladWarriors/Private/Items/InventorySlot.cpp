#include "Items/InventorySlot.h"
#include "Items/InventoryBase.h"
#include "Items/ItemInstanceBase.h"

UInventoryBase* UInventorySlot::GetInventoryOwner() const { return InventoryOwner.Get(); }
UItemInstanceBase* UInventorySlot::GetItem() const { return Item.Get(); }
