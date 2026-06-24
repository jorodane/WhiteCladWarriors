// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ValueContainer.generated.h"

class UValueObject;
class UFloatValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UValueContainer : public UActorComponent
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
	UFloatValue* AddNumberObject(FName WantTag, TSubclassOf<UFloatValue> Template);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void RemoveNumberObject(FName WantTag);

	UFUNCTION(BlueprintCallable, Category = "Container")
	TArray<UFloatValue*> FindAllValueOjbect();

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFloatValue* FindNumberObject(FName WantTag);

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFloatValue* FindOrAddNumberObject(FName WantTag, TSubclassOf<UFloatValue> Template);

	UFUNCTION(BlueprintCallable, Category = "Container")
	bool TryFindNumberObject(FName WantTag, UFloatValue*& Result);

	UFUNCTION(BlueprintCallable, Category = "Container")
	UFillableValue* FindFillableValue(FName WantTag);
	UFUNCTION(BlueprintCallable, Category = "Container")
	bool TryFindFillableValue(FName WantTag, UFillableValue*& Result);



public:	
	UFUNCTION(BlueprintCallable, Category = "Container")
	static UValueContainer* GetValueContainer(AActor* From);

	UFUNCTION(BlueprintCallable, Category = "Container")
	static UValueContainer* GetOrAddValueContainer(AActor* From);
};
