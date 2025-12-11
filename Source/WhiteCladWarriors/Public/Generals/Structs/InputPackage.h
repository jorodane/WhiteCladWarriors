// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputPackage.generated.h"


USTRUCT(BlueprintType)
struct FInputPackage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector DragStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector MouseTerrainPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> MouseClickActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<AActor*> SelectedActors;

};