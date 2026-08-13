// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Generals/Structs/ActionStructures.h"
#include "ActionSpawnable.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSpawnedActorDestroyed, AActor*, DestroyedActor, const FActionCursorFinder&, BaseCursor);

class UActionSpawnNode;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActionSpawnable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IActionSpawnable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ActionSpawnable")
	void SpawnInitialize(UActionSpawnNode* SpawnedNode, const FActionCursorFinder& Cursor, const FOnSpawnedActorDestroyed& OnDestroyEvent);
	void SpawnInitialize_Implementation(UActionSpawnNode* SpawnedNode, const FActionCursorFinder& Cursor, const FOnSpawnedActorDestroyed& OnDestroyEvent) {}
};
