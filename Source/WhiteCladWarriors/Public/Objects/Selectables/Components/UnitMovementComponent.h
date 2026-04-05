// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "NavigationPath.h"
#include "UnitMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class WHITECLADWARRIORS_API UUnitMovementComponent : public UUnitActionComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Navigation")
	TObjectPtr<UNavigationPath> CurrentPath;

	TArray<double> DistanceArray;

	double PathLength = 0.0;

	double MovedDistance = 0.0;

public:
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void SetPath(UNavigationPath* NewPath);

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	bool MoveNextDistance(double WantDistance);

	UFUNCTION(BlueprintPure, Category = "Navigation")
	FVector GetPathLocationWithDistance(double Distance, FVector CurrentLocation);

	UFUNCTION(BlueprintPure, Category = "Navigation")
	FVector GetPathLocationWithPercent(double Percent, FVector CurrentLocation);

	UFUNCTION()
	void OnPathUpdated(UNavigationPath* AffectedPath, TEnumAsByte<ENavPathEvent::Type> PathEvent);
};
