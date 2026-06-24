// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ValueContainer.generated.h"

class UFillableValue;
class UFloatValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UValueContainer : public UActorComponent
{
	GENERATED_BODY()

protected:
	TMap<FName, UFillableValue*> FillValueMap;

	TMap<FName, UFloatValue*> FloatValueMap;

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	float SetFloatValue(FName WantTag, float Value);
	UFUNCTION(BlueprintCallable, Category = "Container")
	float AddFloatValue(FName WantTag, float Value);
	UFUNCTION(BlueprintCallable, Category = "Container")
	float GetFloatValue(FName WantTag, float DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "Container")
	bool AddFillableValue(FName WantTag, UFillableValue* Target);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void RemoveFillableValue(FName WantTag);
	UFUNCTION(BlueprintCallable, Category = "Container")
	UFillableValue* FindFillableValue(FName WantTag);
	UFUNCTION(BlueprintCallable, Category = "Container")
	bool TryFindFillableValue(FName WantTag, UFillableValue*& Result);
	UFUNCTION(BlueprintCallable, Category = "Container")
	TArray<UFillableValue*> FindAllFillableValue();


public:	
	UFUNCTION(BlueprintCallable, Category = "Container")
	static UValueContainer* GetValueContainer(AActor* From);

	UFUNCTION(BlueprintCallable, Category = "Container")
	static UValueContainer* GetOrAddValueContainer(AActor* From);
};
