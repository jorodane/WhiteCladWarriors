// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generals/Structs/InputPackage.h"
#include "ActionBase.generated.h"

class UActionNode;
class AOperator;

UCLASS( Blueprintable, BlueprintType )
class WHITECLADWARRIORS_API AActionBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionNode> RootNode;

protected:
	UPROPERTY(EditAnywhere, Category = "Action")
	FKey ActionHotKey;

	UPROPERTY(EditAnywhere, Category = "Action")
	FName ActionNameDefine;

	UPROPERTY(EditAnywhere, Category = "Action")
	FText ActionNameUI;

	UPROPERTY(EditAnywhere, Category = "Action")
	FSlateBrush ActionIcon;

	UPROPERTY(EditAnywhere, Category = "Action")
	float ActionCooldown;

	UPROPERTY(EditAnywhere, Category = "Action")
	int ActionMaxStack;

	UPROPERTY(EditAnywhere, Category = "Action")
	int UIOrder;

	UPROPERTY(EditAnywhere, Category = "Action")
	bool bShowStack;

public:
	UFUNCTION(BlueprintPure, Category = "Action")
	inline FKey		GetHotKey() { return ActionHotKey; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FKey		SetHotKey(FKey Value) { return ActionHotKey = Value; }


	UFUNCTION(BlueprintPure, Category = "Action")
	inline FName		GetActionNameDefine() { return ActionNameDefine; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FName		SetActionNameDefine(FName Value) { return ActionNameDefine = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline FText		GetActionNameUI()				{ return ActionNameUI;}
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FText		SetActionNameUI(FText Value)	{ return ActionNameUI = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline FSlateBrush	GetActionIcon()				{ return ActionIcon; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FSlateBrush	SetActionIcon(FSlateBrush Value) { return ActionIcon = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline float		GetActionCooldown()			{ return ActionCooldown; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline float		SetActionCooldown(float Value)	{ return ActionCooldown = Value;}

	UFUNCTION(BlueprintPure, Category = "Action")
	inline int			GetActionMaxStack()			{ return ActionMaxStack; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline int			SetActionMaxStack(int Value) { return ActionMaxStack = Value; }

	UFUNCTION(BlueprintPure, Category = "Action")
	inline int			GetUIOrder() { return UIOrder; }
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline int			SetUIOrder(int Value) { return UIOrder = Value; }

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Action")
	int					GetSimpleActionOrder(const FInputPackage& CurrentInput, UUnitActionComponent* CurrentTarget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Action")
	void ExecuteAction(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Action")
	void ExecuteActionWithInput(AOperator* TargetOperator, const TArray<UUnitActionComponent*>& TargetComponents, const FInputPackage& Input);
//public:	
//	// Sets default values for this actor's properties
//	AActionBase();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
