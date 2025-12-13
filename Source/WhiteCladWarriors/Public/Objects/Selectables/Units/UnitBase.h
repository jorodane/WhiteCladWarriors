// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Styling/SlateBrush.h"
#include "Interfaces/Selectable.h"
#include "UnitBase.generated.h"

class AActionBase;
class UUnitActionComponent;
struct FInputPackage;

UCLASS()
class WHITECLADWARRIORS_API AUnitBase : public ACharacter, public ISelectable
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Select")
	FSlateBrush SelectedIcon;

	TMap<AActionBase*, TArray<UUnitActionComponent*>>  ActionMap;

	protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<AActionBase*> GetActionList() const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool GetSimpleAction(const FInputPackage& CurrentInput, AActionBase*& OutAction, TArray<UUnitActionComponent*>& OutComponents) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddActionComponent(UUnitActionComponent* NewComponent);

public:
	bool IsSelectable_Implementation(class AOperator* Operator) { return true; }
	//void Select_Implementation(class AOperator* Operator, bool bIsSingleSelection);
	//void Deselect_Implementation();
	FSlateBrush GetSelectedIcon_Implementation() { return SelectedIcon; }

//public:
//	// Sets default values for this character's properties
//	AUnitBase();
//

//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
