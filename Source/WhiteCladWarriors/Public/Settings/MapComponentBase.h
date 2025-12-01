// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UMapComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:	
	UFUNCTION(BlueprintCallable, Category = "MapComponent")
	virtual void OnAttached(AMapSetting* Owner) {};

	UFUNCTION(BlueprintCallable, Category = "MapComponent")
	virtual void OnDetached(AMapSetting* Owner) {};
};
