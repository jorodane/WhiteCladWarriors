// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MapSettingConnectable.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMapSettingConnectable : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class WHITECLADWARRIORS_API IMapSettingConnectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "MapSetting")
	void OnAttached(AMapSetting* NewOwner);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "MapSetting")
	void OnDetached(AMapSetting* OldOwner);
};