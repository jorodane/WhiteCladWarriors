// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/ActionSetting.h"

TObjectPtr<UActionSetting> UActionSetting::CurrentSetting = nullptr;

void UActionSetting::DestroyComponent(bool bPromoteChildren)
{
	if (Owner) IMapSettingConnectable::Execute_OnDetached(this, Owner);
	Super::DestroyComponent(bPromoteChildren);
}

void UActionSetting::OnAttached_Implementation(AMapSetting* NewOwner)
{
	Owner = NewOwner;
	if (CurrentSetting == nullptr)
	{
		CurrentSetting = this;
	}
}
void UActionSetting::OnDetached_Implementation(AMapSetting* OldOwner)
{
	if (OldOwner == Owner) Owner = nullptr;
	if (CurrentSetting == this)
	{
		CurrentSetting = nullptr;
	}
}

void UActionSetting::InitiateActions_Implementation(const FMapInfo& WantInfo)
{

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

