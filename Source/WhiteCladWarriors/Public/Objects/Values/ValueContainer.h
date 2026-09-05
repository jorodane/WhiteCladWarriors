// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InfoConnectable.h"
#include "ValueContainer.generated.h"

class UValueObject;
class UFloatValue;
class UFillableValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UValueContainer : public UActorComponent, public IInfoConnectable
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, TObjectPtr<UFloatValue>> NumberValueMap;

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	float SetNumber(FName WantTag, float Value);
	UFUNCTION(BlueprintCallable, Category = "Container")
	float AddNumber(FName WantTag, float Value);
	UFUNCTION(BlueprintCallable, Category = "Container")
	float GetNumber(FName WantTag, float DefaultValue);

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFloatValue* AddNumberObject(FName WantTag, float InitialValue, TSubclassOf<UFloatValue> Template);

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFillableValue* AddFillableObject(FName WantTag, float InitialValue, float MaxValue, TSubclassOf<UFillableValue> Template);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void RemoveNumberObject(FName WantTag);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void RemoveFillableObject(FName WantTag) { RemoveNumberObject(WantTag); }

	UFUNCTION(BlueprintCallable, Category = "Container")
	TArray<UValueObject*> FindAllValueOjbect() const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFloatValue* FindNumberObject(FName WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFloatValue* FindOrAddNumberObject(FName WantTag, float DefaultValue, TSubclassOf<UFloatValue> Template);

	UFUNCTION(BlueprintCallable, Category = "Container", meta = (ExpandEnumAsExecs = "ReturnValue"))
	bool TryFindNumberObject(FName WantTag, UFloatValue*& Result);

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFillableValue* FindFillableObject(FName WantTag) const;

	UFUNCTION(BlueprintCallable, Category = "Container", meta = (ExpandEnumAsExecs = "ReturnValue"))
	bool TryFindFillableObject(FName WantTag, UFillableValue*& Result);

public:
	virtual TArray<UOrderedGenericWidgetClaim*> GetInfoWidget_Implementation(EInfoWidgetType WantType, AOperator* Operator) const;


public:	
	UFUNCTION(BlueprintCallable, Category = "Container", Meta = (DefaultToSelf = "From"))
	static UValueContainer* GetValueContainer(AActor* From);

	UFUNCTION(BlueprintCallable, Category = "Container", Meta = (DefaultToSelf = "From", ExpandEnumAsExecs = "ReturnValue"))
	static bool TryGetValueContainer(AActor* From, UValueContainer*& Result);

	UFUNCTION(BlueprintCallable, Category = "Container", Meta = (DefaultToSelf = "From"))
	static UValueContainer* GetOrAddValueContainer(AActor* From);
};
