
#include "Generals/Structs/ActionValueContainer.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"

void FActionValueContainer::Clear()
{
	Hierarchy.Reset();
	ComponentMap.Reset();
	Values.Reset();
}

int FActionValueContainer::Registration(int ParentID)
{
	int NewID = NextID++;
	Hierarchy.Add(NewID, ParentID);
	return NewID;
}

int FActionValueContainer::Registration(UUnitActionComponent* Component, int ParentID)
{
	if (!IsValid(Component)) return InvalidID;
	int* FindedID = ComponentMap.Find(Component);
	if (FindedID != nullptr) return *FindedID;

	ParentID = FMath::Max(0, ParentID);
	int NewID = Registration(ParentID);
	ComponentMap.Add(Component, NewID);
	return NewID;
}

FName FActionValueContainer::GetValueKey(int StartID, const FName& Tag) const
{
	return FName(Tag, StartID);
}

bool FActionValueContainer::HasValue(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, int& OutFoundID) const
{
	const FPropertyBagPropertyDesc* CurrentDescriptor = nullptr;
	return GetValueDescriptor(StartID, Tag, PropertyType, CurrentDescriptor, OutFoundID);
}

bool FActionValueContainer::HasLocalValue(int TargetID, const FName& Tag) const
{
	return GetLocalValueDescriptor(TargetID, Tag) != nullptr;
}

bool FActionValueContainer::GetValueDescriptor(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, const FPropertyBagPropertyDesc*& OutDescriptor, int& OutFoundID) const
{
	int CurrentID = StartID;
	while (CurrentID >= RootID)
	{
		const FPropertyBagPropertyDesc* CurrentDescriptor = GetLocalValueDescriptor(CurrentID, Tag);
		
		if (CurrentDescriptor != nullptr && CurrentDescriptor->ValueType == PropertyType)
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

bool FActionValueContainer::GetValueDescriptor(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, const FPropertyBagPropertyDesc*& OutDescriptor) const
{
	int FoundID = InvalidID;
	return GetValueDescriptor(StartID, Tag, PropertyType, OutDescriptor, FoundID);
}

const FPropertyBagPropertyDesc* FActionValueContainer::GetLocalValueDescriptor(int TargetID, const FName& Tag) const
{
	return Values.FindPropertyDescByName(GetValueKey(TargetID, Tag));
}

bool FActionValueContainer::GetClass(int StartID, const FName& Tag, UClass*& OutResult) const
{
	const FPropertyBagPropertyDesc* Descriptor = nullptr;
	if (GetValueDescriptor(StartID, Tag, EPropertyBagPropertyType::Class, Descriptor))
	{
		TValueOrError<UClass*, EPropertyBagResult> Result = Values.GetValueClass(*Descriptor);
		if (Result.IsValid())
		{
			OutResult = Result.GetValue();
			return true;
		}
	}
	OutResult = nullptr;
	return false;
}