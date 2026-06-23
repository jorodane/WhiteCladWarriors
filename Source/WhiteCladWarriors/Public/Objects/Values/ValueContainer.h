// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ValueContainer.generated.h"

class UFillableValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UValueContainer : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	UFillableValue* AddFillableValue();

public:	
	UFUNCTION(BlueprintCallable, Category = "Container")
	static UValueContainer* GetValueContainer(AActor* From);

	UFUNCTION(BlueprintCallable, Category = "Container")
	static UValueContainer* GetOrAddValueContainer(AActor* From);
};
