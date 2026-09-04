
#include "Generals/Structs/ValueContainer.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

void FValueContainer::Clear()
{
	Hierarchy.Reset();
	ComponentMap.Reset();
	Values.Reset();
}

int FValueContainer::Registration(int ParentID)
{
	int NewID = NextID++;
	Hierarchy.Add(NewID, ParentID);
	return NewID;
}

int FValueContainer::Registration(UUnitActionComponent* Component, int ParentID)
{
	if (!IsValid(Component)) return InvalidID;
	int NewID = Registration(ParentID);
	ComponentMap.Add(Component, NewID);
	return NewID;
}

FName FValueContainer::GetValueKey(int StartID, const FName& Tag) const
{
	return FName(Tag, StartID);
}

bool FValueContainer::HasValue(int StartID, const FName& Tag, int& OutFoundID) const
{
	const FPropertyBagPropertyDesc* CurrentDescriptor = nullptr;
	return GetValueDescriptor(StartID, Tag, CurrentDescriptor, OutFoundID);
}

bool FValueContainer::HasLocalValue(int TargetID, const FName& Tag) const
{
	return GetLocalValueDescriptor(TargetID, Tag) != nullptr;
}

bool FValueContainer::GetValueDescriptor(int StartID, const FName& Tag, const FPropertyBagPropertyDesc*& OutDescriptor, int& OutFoundID) const
{
	int CurrentID = StartID;
	while (CurrentID >= RootID)
	{
		const FPropertyBagPropertyDesc* CurrentDescriptor = GetLocalValueDescriptor(CurrentID, Tag);
		if (CurrentDescriptor != nullptr)
		{
			OutFoundID = CurrentID;
			OutDescriptor = CurrentDescriptor;
			return true;
		}
		else if (CurrentID <= RootID) break;
		const int* ParentID = Hierarchy.Find(CurrentID);
		if (ParentID == nullptr) CurrentID = RootID;
		else CurrentID = *ParentID;
	}
	OutFoundID = InvalidID;
	OutDescriptor = nullptr;
	return false;
}

const FPropertyBagPropertyDesc* FValueContainer::GetLocalValueDescriptor(int TargetID, const FName& Tag) const
{
	return Values.FindPropertyDescByName(GetValueKey(TargetID, Tag));
}