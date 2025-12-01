// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/ActionSetting.h"

TObjectPtr<UActionSetting> UActionSetting::CurrentSetting = nullptr;

void UActionSetting::OnAttached(AMapSetting* Owner)
{
	if (CurrentSetting == nullptr)
	{
		CurrentSetting = this;
	}
}
void UActionSetting::OnDetached(AMapSetting* Owner)
{
	if (CurrentSetting == this)
	{
		CurrentSetting = nullptr;
	}
}

AActionBase* UActionSetting::GetAction(FName WantName)
{
	if (CurrentSetting)
	{
		if (AActionBase** Result = CurrentSetting->ActionList.Find(WantName))
		{
			return *Result;
		}
	}
	return nullptr;
}
//// Sets default values for this component's properties
//UActionSetting::UActionSetting()
//{
//	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
//	// off to improve performance if you don't need them.
//	PrimaryComponentTick.bCanEverTick = true;
//
//	// ...
//}
//
//
//// Called when the game starts
//void UActionSetting::BeginPlay()
//{
//	Super::BeginPlay();
//
//	// ...
//	
//}
//
//
//// Called every frame
//void UActionSetting::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

