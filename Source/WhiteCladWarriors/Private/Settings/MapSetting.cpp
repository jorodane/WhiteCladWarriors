// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/MapSetting.h"
#include "Interfaces/MapSettingConnectable.h"

TObjectPtr<AMapSetting> AMapSetting::CurrentSetting = nullptr;

const FVector2D AMapSetting::DefaultMapHalfSize = DEFAULT_MAP_HALFSIZE;
const FVector2D AMapSetting::DefaultMapSize = DEFAULT_MAP_SIZE;
const FString AMapSetting::DefaultMapName = DEFAULT_MISSING_NAME;

FString AMapSetting::GetCurrentMapName() { return CurrentSetting ? CurrentSetting->Info.MapName : DefaultMapName; }
FVector2D AMapSetting::GetCurrentMapHalfSize() { return CurrentSetting ? CurrentSetting->Info.MapHalfSize : DefaultMapHalfSize; }
FVector2D AMapSetting::GetCurrentMapSize() { return CurrentSetting ? CurrentSetting->Info.MapHalfSize * 2 : DefaultMapSize; }


void AMapSetting::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	if (CurrentSetting)
	{
		Destroy();
	}
	else
	{
		CurrentSetting = this;
		
		for (UActorComponent* CurrentComponent : GetComponentsByInterface(UMapSettingConnectable::StaticClass()))
		{
			IMapSettingConnectable::Execute_OnAttached(CurrentComponent, this);
		}
	}
}

void AMapSetting::BeginDestroy()
{
	if (CurrentSetting == this) CurrentSetting = nullptr;
	Super::BeginDestroy();
}

FVector AMapSetting::MapOffsetToPosition(FVector2D Offset, bool Clamped01, bool InvertX, bool InvertY)
{
	if (Clamped01)
	{
		Offset.X -= 0.5f;
		Offset.Y -= 0.5f;
	}
	FVector2D MapHalf = GetCurrentMapHalfSize();
	FVector Result = FVector();
	Result.Z = 0.0f;
	Result.X = MapHalf.Y * Offset.Y;
	Result.Y = MapHalf.X * Offset.X;
	if (InvertX) Result.X *= -1.0f;
	if (InvertY) Result.Y *= -1.0f;
	return Result;
}


FVector2D AMapSetting::PositionToMapOffset(FVector Position, bool Clamped01, bool InvertX, bool InvertY)
{
	FVector2D MapHalf = GetCurrentMapSize();
	FVector2D Result = FVector2D();
	Result.X = Position.Y / MapHalf.Y;
	Result.Y = Position.X / MapHalf.X;
	if (InvertX) Result.X *= -1.0f;
	if (InvertY) Result.Y *= -1.0f;

	if (Clamped01)
	{
		Result.X += 0.5f;
		Result.Y += 0.5f;
	}

	return Result;
}
