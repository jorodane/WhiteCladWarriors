// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Selectable.h"
#include "UnitBase.generated.h"

UCLASS()
class WHITECLADWARRIORS_API AUnitBase : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	bool IsSelectable_Implementation(class AOperator* Operator);
	void Select_Implementation(class AOperator* Operator, bool bIsSingleSelection);

//public:
//	// Sets default values for this character's properties
//	AUnitBase();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
