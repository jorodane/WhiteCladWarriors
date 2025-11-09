// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/MapSetting.h"

AMapSetting* AMapSetting::CurrentSetting = nullptr;

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
	}
}

void AMapSetting::BeginDestroy()
{
	Super::BeginDestroy();
	if (CurrentSetting == this) CurrentSetting = nullptr;
}