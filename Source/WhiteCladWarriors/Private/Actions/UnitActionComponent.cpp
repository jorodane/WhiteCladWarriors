// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/UnitActionComponent.h"
#include "Objects/Selectables/Units/UnitBase.h"
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

AUnitBase* UUnitActionComponent::GetOwnerUnit_Implementation() { return Cast<AUnitBase>(GetOwner()); }

bool UUnitActionComponent::GetMainActionCancelable()
{
	if (AUnitBase* OwnerUnit = GetOwnerUnit()) return OwnerUnit->GetMainActionCancelable();
	return false;
}

bool UUnitActionComponent::TrySetMainAction_Implementation(UActionExecutor* Executor, bool bIsCancelable)
{
	if (AUnitBase* OwnerUnit = GetOwnerUnit()) return OwnerUnit->SetMainAction(Executor, this, bIsCancelable);
	return false;
}

void UUnitActionComponent::EndMainAction_Implementation(UActionExecutor* Executor)
{
	if (AUnitBase* OwnerUnit = GetOwnerUnit()) OwnerUnit->EndMainAction(Executor);
}