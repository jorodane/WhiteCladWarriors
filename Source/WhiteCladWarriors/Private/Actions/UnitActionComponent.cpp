// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/UnitActionComponent.h"
#include "Kismet/KismetMathLibrary.h"

AActor* UUnitActionComponent::SpawnActor_Implementation(TSubclassOf<AActor> TemplateClass)
{
	if(UWorld* WorldContext = GetWorld()) return WorldContext->SpawnActor(TemplateClass);
	return nullptr;
}

FVector UUnitActionComponent::GetLocation_Implementation()
{
	if (AActor* Owner = GetOwner()) return Owner->GetActorLocation();
	return FVector::ZeroVector;
};

FVector UUnitActionComponent::GetDirection_Implementation(FVector Destination, bool bIsIgnoreZ)
{
	FVector Result = Destination - GetLocation();
	if (bIsIgnoreZ) Result = Result.GetSafeNormal2D();
	else Result = Result.GetSafeNormal();
	return Result;
};
