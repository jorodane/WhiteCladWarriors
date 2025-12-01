// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapSetting.generated.h"

#define DEFAULT_MAP_NAME TEXT("UNNAMED")
#define DEFAULT_MAP_LENGTH 50400.0f
#define DEFAULT_MAP_SIZE FVector2D(DEFAULT_MAP_LENGTH, DEFAULT_MAP_LENGTH);
#define DEFAULT_MAP_HALFLENGTH 25200.0f
#define DEFAULT_MAP_HALFSIZE FVector2D(DEFAULT_MAP_HALFLENGTH, DEFAULT_MAP_HALFLENGTH);

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
	static TObjectPtr<AMapSetting> CurrentSetting;

public:
	static const FVector2D DefaultMapHalfSize;
	static const FVector2D DefaultMapSize;
	static const FString DefaultMapName;

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
