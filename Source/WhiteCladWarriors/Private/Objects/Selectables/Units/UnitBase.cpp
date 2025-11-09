// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Units/UnitBase.h"

bool AUnitBase::IsSelectable_Implementation(class AOperator* Operator)
{ 
	return true;
}

void AUnitBase::Select_Implementation(class AOperator* Operator, bool bIsSingleSelection)
{

}

void AUnitBase::Deselect_Implementation()
{

}

//// Sets default values
//AUnitBase::AUnitBase()
//{
// 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//}
//
//// Called when the game starts or when spawned
//void AUnitBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void AUnitBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//// Called to bind functionality to input
//void AUnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

