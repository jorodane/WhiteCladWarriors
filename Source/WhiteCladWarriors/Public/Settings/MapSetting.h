// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapSetting.generated.h"

#define DEFAULT_MAP_NAME TEXT("UNNAMED")
#define DEFAULT_MAP_SIZE FVector2D::UnitVector * 50400.0f
#define DEFAULT_MAP_HALFSIZE FVector2D::UnitVector * 25200.0f

USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapSetting")
	FString MapName = DEFAULT_MAP_NAME;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapSetting")
	FVector2D MapHalfSize = DEFAULT_MAP_HALFSIZE;
};

UCLASS()
class WHITECLADWARRIORS_API AMapSetting : public AActor
{
	GENERATED_BODY()
	
private:
	static AMapSetting* CurrentSetting;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapSetting")
	FMapInfo Info;

public:	
	virtual void PreInitializeComponents() override;
	virtual void BeginDestroy() override;

public:
	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static AMapSetting* GetCurrentMapSetting() { return CurrentSetting; }

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FString GetCurrentMapName() { return CurrentSetting ? CurrentSetting->Info.MapName : DEFAULT_MAP_NAME; }

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FVector2D GetCurrentMapHalfSize() { return CurrentSetting ? CurrentSetting->Info.MapHalfSize : DEFAULT_MAP_HALFSIZE; }

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FVector2D GetCurrentMapSize() { return CurrentSetting ? CurrentSetting->Info.MapHalfSize * 2 : DEFAULT_MAP_SIZE; }
};
