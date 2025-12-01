#pragma once

#include "CoreMinimal.h"
#include "MapInfo.generated.h"

#define DEFAULT_MISSING_NAME TEXT("UNNAMED")
#define DEFAULT_MAP_LENGTH 50400.0f
#define DEFAULT_MAP_SIZE FVector2D(DEFAULT_MAP_LENGTH, DEFAULT_MAP_LENGTH);
#define DEFAULT_MAP_HALFLENGTH 25200.0f
#define DEFAULT_MAP_HALFSIZE FVector2D(DEFAULT_MAP_HALFLENGTH, DEFAULT_MAP_HALFLENGTH);

USTRUCT(BlueprintType)
struct FActionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapSetting")
	FString ActionName = DEFAULT_MISSING_NAME;
};

USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapSetting")
	FString MapName = DEFAULT_MISSING_NAME;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapSetting")
	FVector2D MapHalfSize = DEFAULT_MAP_HALFSIZE;
};