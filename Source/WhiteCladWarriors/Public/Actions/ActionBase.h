// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionBase.generated.h"

class UActionNode;

UCLASS( Blueprintable, BlueprintType )
class WHITECLADWARRIORS_API AActionBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<UActionNode> RootNode;

protected:
	UPROPERTY(EditAnywhere, Category = "Action")
	FText ActionName;

	UPROPERTY(EditAnywhere, Category = "Action")
	FSlateBrush ActionIcon;

	UPROPERTY(EditAnywhere, Category = "Action")
	float ActionCooldown;

	UPROPERTY(EditAnywhere, Category = "Action")
	int ActionMaxStack;

	UPROPERTY(EditAnywhere, Category = "Action")
	int UIOrder;

public:
	UFUNCTION(BlueprintPure, Category = "Action")
	inline FText		GetActionName()				{ return ActionName;}
	UFUNCTION(BlueprintCallable, Category = "Action")
	inline FText		SetActionName(FText Value)	{ return ActionName = Value; }

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
