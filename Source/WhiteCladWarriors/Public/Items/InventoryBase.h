// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryBase.generated.h"

class UInventorySlot;
class UItemBase;
class UItemInstanceBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UInventoryBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<UInventorySlot*> SlotArray;

	TMultiMap<UItemInstanceBase*, UInventorySlot*> ItemArray;
};
