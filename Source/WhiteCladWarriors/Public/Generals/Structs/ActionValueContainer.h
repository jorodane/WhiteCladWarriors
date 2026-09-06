// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/PropertyBag.h"
#include "ActionValueConatainer.generated.h"
/**
 * 
 */
class UUnitActionComponent;

USTRUCT(BlueprintType)
struct FActionValueContainer
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

	bool HasValue(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, int& OutFoundID) const;
	bool HasLocalValue(int TargetID, const FName& Tag) const;
	bool GetValueDescriptor(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, const FPropertyBagPropertyDesc*& OutDescriptor, int& OutFoundID) const;
	bool GetValueDescriptor(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, const FPropertyBagPropertyDesc*& OutDescriptor) const;
	const FPropertyBagPropertyDesc* GetLocalValueDescriptor(int TargetID, const FName& Tag) const;

	template <typename T, typename GetValueFunction>
	bool GetValue(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, T& OutResult, const T& DefaultValue, GetValueFunction Getter) const
	{
		const FPropertyBagPropertyDesc* Descriptor = nullptr;
		if (GetValueDescriptor(StartID, Tag, PropertyType, Descriptor))
		{
			TValueOrError<T, EPropertyBagResult> Result = Getter(Values, *Descriptor);
			if (Result.IsValid())
			{
				OutResult = Result.GetValue();
				return true;
			}
		}
		OutResult = DefaultValue;
		return false;
	}

	template <typename T>
	bool GetStruct(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, T& OutResult, const T& DefaultValue) const
	{
		int CurrentID = StartID;

		while (CurrentID >= RootID)
		{
			const FPropertyBagPropertyDesc* Descriptor = GetLocalValueDescriptor(CurrentID, Tag);

			if (Descriptor != nullptr)
			{
				TValueOrError<T*, EPropertyBagResult> Result = Values.GetValueStruct<T>(*Descriptor);

				if (Result.IsValid())
				{
					OutResult = *Result.GetValue();
					return true;
				}
			}

			if (CurrentID <= RootID) break;
			const int* ParentID = Hierarchy.Find(CurrentID);
			CurrentID = ParentID ? *ParentID : RootID;
		}

		OutResult = DefaultValue;
		return false;
	}

	template <typename T>
	bool GetObject(int StartID, const FName& Tag, EPropertyBagPropertyType PropertyType, T*& OutResult) const
	{
		int CurrentID = StartID;

		while (CurrentID >= RootID)
		{
			const FPropertyBagPropertyDesc* Descriptor = GetLocalValueDescriptor(CurrentID, Tag);

			if (Descriptor != nullptr)
			{
				TValueOrError<T*, EPropertyBagResult> Result = Values.GetValueObject<T>(*Descriptor);

				if (Result.IsValid())
				{
					OutResult = *Result.GetValue();
					return true;
				}
			}

			if (CurrentID <= RootID) break;
			const int* ParentID = Hierarchy.Find(CurrentID);
			CurrentID = ParentID ? *ParentID : RootID;
		}

		OutResult = DefaultValue;
		return false;
	}

	bool GetClass(int StartID, const FName& Tag, UClass*& OutResult) const;

	bool GetSoftObjectPath(int StartID, const FName& Tag, FSoftObjectPath& OutResult, const FSoftObjectPath& DefaultValue) const
	{
		return GetValue<FSoftObjectPath>(StartID, Tag, EPropertyBagPropertyType::SoftObject, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueSoftPath(Descriptor); });
	}

	bool GetBoolean(int StartID, const FName& Tag, bool& OutResult, bool DefaultValue = false) const
	{
		return GetValue<bool>(StartID, Tag, EPropertyBagPropertyType::Bool, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueBool(Descriptor); });
	}

	bool GetFloat(int StartID, const FName& Tag, float& OutResult, float DefaultValue = 0.0f) const
	{
		return GetValue<float>(StartID, Tag, EPropertyBagPropertyType::Float, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueFloat(Descriptor); });
	}

	bool GetDouble(int StartID, const FName& Tag, double& OutResult, double DefaultValue = 0.0) const
	{
		return GetValue<double>(StartID, Tag, EPropertyBagPropertyType::Double, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueDouble(Descriptor); });
	}

	bool GetInteger32(int StartID, const FName& Tag, int32& OutResult, int32 DefaultValue = 0) const
	{
		return GetValue<int32>(StartID, Tag, EPropertyBagPropertyType::Int32, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueInt32(Descriptor); });
	}

	bool GetInteger64(int StartID, const FName& Tag, int64& OutResult, int64 DefaultValue = 0) const
	{
		return GetValue<int64>(StartID, Tag, EPropertyBagPropertyType::Int64, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueInt64(Descriptor); });
	}

	bool GetUnsignedInteger32(int StartID, const FName& Tag, uint32& OutResult, uint32 DefaultValue = 0) const
	{
		return GetValue<uint32>(StartID, Tag, EPropertyBagPropertyType::UInt32, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueUInt32(Descriptor); });
	}

	bool GetUnsignedInteger64(int StartID, const FName& Tag, uint64& OutResult, uint64 DefaultValue = 0) const
	{
		return GetValue<uint64>(StartID, Tag, EPropertyBagPropertyType::UInt64, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueUInt64(Descriptor); });
	}

	bool GetByte(int StartID, const FName& Tag, uint8& OutResult, uint8 DefaultValue = 0) const
	{
		return GetValue<uint8>(StartID, Tag, EPropertyBagPropertyType::Byte, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueByte(Descriptor); });
	}

	bool GetName(int StartID, const FName& Tag, FName& OutResult, const FName& DefaultValue = NAME_None) const
	{
		return GetValue<FName>(StartID, Tag, EPropertyBagPropertyType::Name, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueName(Descriptor); });
	}

	bool GetString(int StartID, const FName& Tag, FString& OutResult, const FString& DefaultValue = TEXT("")) const
	{
		return GetValue<FString>(StartID, Tag, EPropertyBagPropertyType::String, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueString(Descriptor); });
	}

	bool GetText(int StartID, const FName& Tag, FText& OutResult, const FText& DefaultValue = FText::GetEmpty()) const
	{
		return GetValue<FText>(StartID, Tag, EPropertyBagPropertyType::Text, OutResult, DefaultValue, [](const FInstancedPropertyBag& Values, const FPropertyBagPropertyDesc& Descriptor) {return Values.GetValueText(Descriptor); });
	}

	template<typename T, typename SetValueFunction>
	void SetValue(int ID, const FName& Tag, EPropertyBagPropertyType PropertyType, const T& Value, SetValueFunction Setter)
	{
		const FName Key = GetValueKey(ID, Tag);
		if (!Values.FindPropertyDescByName(Key)) Values.AddProperty(Key, PropertyType);
		Setter(Values, Key, Value);
	}

	template <typename T>
	void SetStruct(int ID, const FName& Tag, const T& Value)
	{
		const FName Key = GetValueKey(ID, Tag);
		if (!Values.FindPropertyDescByName(Key)) Values.AddProperty(Key, EPropertyBagPropertyType::Struct, StaticStruct<T>());
		Values.SetValueStruct<T>(Key, Value);
	}

	template <typename T>
	void SetObject(int ID, const FName& Tag, T* Value) 
	{
		const FName Key = GetValueKey(ID, Tag);
		if (!Values.FindPropertyDescByName(Key)) Values.AddProperty(Key, EPropertyBagPropertyType::Object, T::StaticClass());
		Values.SetValueObject<T>(Key, Value);
	}
	void SetClass(int ID, const FName& Tag, UClass* Value) 
	{ 
		SetValue(ID, Tag, EPropertyBagPropertyType::Class, Value, [](FInstancedPropertyBag& Values, const FName& Key, UClass* Value) { Values.SetValueClass(Key, Value); });
	}
	void SetSoftObjectPath(int ID, const FName& Tag, const FSoftObjectPath& Value) 
	{ 
		SetValue(ID, Tag, EPropertyBagPropertyType::SoftObject, Value, [](FInstancedPropertyBag& Values, const FName& Key, const FSoftObjectPath& Value) { Values.SetValueSoftPath(Key, Value); });
	}
	void SetBoolean(int ID, const FName& Tag, bool Value) 
	{ 
		SetValue(ID, Tag, EPropertyBagPropertyType::Bool, Value, [](FInstancedPropertyBag& Values, const FName& Key, bool Value) { Values.SetValueBool(Key, Value); });
	}
	void SetFloat(int ID, const FName& Tag, float Value)  
	{ 
		SetValue(ID, Tag, EPropertyBagPropertyType::Float, Value, [](FInstancedPropertyBag& Values, const FName& Key, float Value) { Values.SetValueFloat(Key, Value); });
	}
	void SetDouble(int ID, const FName& Tag, double Value) 
	{ 
		SetValue(ID, Tag, EPropertyBagPropertyType::Double, Value, [](FInstancedPropertyBag& Values, const FName& Key, double Value) { Values.SetValueDouble(Key, Value); });
	}
	void SetInteger32(int ID, const FName& Tag, int32 Value) 
	{ 
		SetValue(ID, Tag, EPropertyBagPropertyType::Int32, Value, [](FInstancedPropertyBag& Values, const FName& Key, int32 Value) { Values.SetValueInt32(Key, Value); });
	}
	void SetInteger64(int ID, const FName & Tag, int64 Value)
	{
		SetValue(ID, Tag, EPropertyBagPropertyType::Int64, Value, [](FInstancedPropertyBag& Values, const FName& Key, int64 Value) { Values.SetValueInt64(Key, Value); });
	}
	void SetUnsignedInteger32(int ID, const FName & Tag, uint32 Value)
	{
		SetValue(ID, Tag, EPropertyBagPropertyType::UInt32, Value, [](FInstancedPropertyBag& Values, const FName& Key, uint32 Value) { Values.SetValueUInt32(Key, Value); });
	}
	void SetUnsignedInteger64(int ID, const FName & Tag, uint64 Value)
	{
		SetValue(ID, Tag, EPropertyBagPropertyType::UInt64, Value, [](FInstancedPropertyBag& Values, const FName& Key, uint64 Value) { Values.SetValueUInt64(Key, Value); });
	}
	void SetByte(int ID, const FName & Tag, uint8 Value)
	{
		SetValue(ID, Tag, EPropertyBagPropertyType::Byte, Value, [](FInstancedPropertyBag& Values, const FName& Key, uint8 Value) { Values.SetValueByte(Key, Value); });
	}
	void SetName(int ID, const FName & Tag, const FName & Value)
	{
		SetValue(ID, Tag, EPropertyBagPropertyType::Name, Value, [](FInstancedPropertyBag& Values, const FName& Key, const FName& Value) { Values.SetValueName(Key, Value); });
	}
	void SetString(int ID, const FName & Tag, const FString & Value)
	{
		SetValue(ID, Tag, EPropertyBagPropertyType::String, Value, [](FInstancedPropertyBag& Values, const FName& Key, const FString& Value) { Values.SetValueString(Key, Value); });
	}
	void SetText(int ID, const FName & Tag, const FText & Value)
	{ 
		SetValue(ID, Tag, EPropertyBagPropertyType::Text, Value, [](FInstancedPropertyBag& Values, const FName& Key, const FText& Value) { Values.SetValueText(Key, Value); });
	}
};
