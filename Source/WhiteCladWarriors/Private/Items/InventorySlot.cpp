#include "Items/InventorySlot.h"
#include "Items/ItemInstanceBase.h"

int UInventorySlot::AddAmount(int Value, const int& MaxAmount)
{
	if (Value <= 0) return 0;
	else if (Amount == MaxAmount) return Value;
	else if (MaxAmount <= 0)
	{
		Amount += Value;
		return 0;
	}
	else if (MaxAmount < Amount)
	{
		int Removed = Amount - MaxAmount;
		Amount = MaxAmount;
		return Value + Removed;
	}
	else
	{
		int Added = FMath::Min(Value, MaxAmount - Amount);
		Amount += Added;
		return Value - Added;
	}
}

int UInventorySlot::RemoveAmount(int Value, bool& OutIsEmpty)
{
	if (Value <= 0)
	{
		OutIsEmpty = Amount <= 0;
		return 0;
	}
	else if (Amount <= Value)
	{
		int Removed = Amount;
		Amount = 0;
		OutIsEmpty = true;
		return Value - Removed;
	}
	else
	{
		Amount -= Value;
		OutIsEmpty = false;
		return Value;
	}
}