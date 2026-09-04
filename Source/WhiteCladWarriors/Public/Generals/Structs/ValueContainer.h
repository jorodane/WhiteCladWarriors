// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/PropertyBag.h"
#include "ValueConatainer.generated.h"
/**
 * 
 */
class UUnitActionComponent;

USTRUCT(BlueprintType)
struct FValueContainer
{
	GENERATED_BODY()

	static constexpr int RootID = 0;
	static constexpr int InvalidID = -1;

	UPROPERTY()
	TMap<int, int> Hierarchy;

	UPROPERTY()
	TMap<UUnitActionComponent*, int> ComponentMap;

	UPROPERTY()
	FInstancedPropertyBag Values;

	UPROPERTY(BlueprintReadOnly)
	int NextID = 1;

	void Clear();

	int Registration(int ParentID = RootID);
	int Registration(UUnitActionComponent* Component, int ParentID = RootID);

	FName GetValueKey(int StartID, const FName& Tag) const;

	bool HasValue(int StartID, const FName& Tag, int& OutFoundID) const;
	bool HasLocalValue(int TargetID, const FName& Tag) const;
	bool GetValueDescriptor(int StartID, const FName& Tag, const FPropertyBagPropertyDesc*& OutDescriptor, int& OutFoundID) const;
	const FPropertyBagPropertyDesc* GetLocalValueDescriptor(int TargetID, const FName& Tag) const;

	template <typename T>
	T GetValue(int StartID, const FName& Tag) const
	{
		const FPropertyBagPropertyDesc* Descriptor = nullptr; 
		int FoundIndex = InvalidID;
		if (GetValueDescriptor(TargetID, Tag, Descriptor, InvalidID))
		{
			if (Descriptor == nullptr) return default(T);
		}
	}
};
