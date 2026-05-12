// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Generals/Structs/DamageStructures.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHITECLADWARRIORS_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float TakeDamage(const FDamageInfo& Info, bool& bIsKill);
	virtual float TakeDamage_Implementation(const FDamageInfo& Info, bool& bIsKill) { return 0.0f; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool GetIsAttackable(UUnitMainComponent* From);
	virtual bool GetIsAttackable_Implementation(UUnitMainComponent* From) { return false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool GetIsDamageable(UUnitMainComponent* From);
	virtual bool GetIsDamageable_Implementation(UUnitMainComponent* From) { return false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool GetIsDie();
	virtual bool GetIsDie_Implementation() { return false; }

};
