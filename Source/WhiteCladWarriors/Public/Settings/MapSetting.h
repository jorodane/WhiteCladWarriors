// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Settings/MapInfo.h"
#include "GameFramework/Actor.h"
#include "MapSetting.generated.h"

UCLASS()
class WHITECLADWARRIORS_API AMapSetting : public AActor
{
	GENERATED_BODY()
	
private:
	static TObjectPtr<AMapSetting> CurrentSetting;

public:
	static const FVector2D DefaultMapHalfSize;
	static const FVector2D DefaultMapSize;
	static const FString DefaultMapName;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapSetting")
	FMapInfo Info;

	UPROPERTY(BlueprintReadWrite, Category = "MapSetting")
	float WorldTime = 0.0f;

public:	
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static AMapSetting* GetCurrentMapSetting() { return CurrentSetting; }

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static float GetCurrentWorldTime();

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FString GetCurrentMapName();

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FVector2D GetCurrentMapHalfSize();

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FVector2D GetCurrentMapSize();

	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FVector MapOffsetToPosition(FVector2D Offset, bool Clamped01 = false, bool InvertX = true, bool InvertY = false);
	UFUNCTION(BlueprintPure, Category = "MapSetting")
	static FVector2D PositionToMapOffset(FVector Position, bool Clamped01 = false, bool InvertX = true, bool InvertY = false);
};
