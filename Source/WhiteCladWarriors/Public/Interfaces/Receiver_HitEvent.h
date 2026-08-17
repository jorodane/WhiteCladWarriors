// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Generals/Structs/ActionStructures.h"
#include "Receiver_HitEvent.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UReceiver_HitEvent : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IReceiver_HitEvent
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hit")
	void OnHitReceived(AActor* From, const FActionCursorFinder& BaseCursor, const FHitResult& Hit);
	void OnHitReceived_Implementation(AActor* From, const FActionCursorFinder& BaseCursor, const FHitResult& Hit) {  }
};
