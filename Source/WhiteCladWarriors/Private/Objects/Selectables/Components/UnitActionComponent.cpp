// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Kismet/KismetMathLibrary.h"

AActor* UUnitActionComponent::SpawnActor_Implementation(TSubclassOf<AActor> TemplateClass)
{
	if(UWorld* WorldContext = GetWorld()) return WorldContext->SpawnActor(TemplateClass);
	return nullptr;
}

bool UUnitActionComponent::GetMainActionCancelable() const
{
	if (UUnitMainComponent* CurrentUnit = GetOwnerUnit()) return CurrentUnit->GetMainActionCancelable();
	return false;
}

bool UUnitActionComponent::TrySetMainAction_Implementation(const FActionCursorFinder& WantCursor, UActionNode* TargetNode)
{
	if (UUnitMainComponent* CurrentUnit = GetOwnerUnit()) return CurrentUnit->SetMainAction(WantCursor, TargetNode);
	return false;
}

void UUnitActionComponent::EndMainAction(int64 ExecutorID, UUnitActionComponent* OldComponent)
{
	if (UUnitMainComponent* CurrentUnit = GetOwnerUnit()) CurrentUnit->EndMainAction(ExecutorID, OldComponent);
}

void UUnitActionComponent::OnInputStart_Implementation(const FInputClaim& StartedInput)
{
	if (UUnitMainComponent* CurrentUnit = GetOwnerUnit()) CurrentUnit->PlayInputReadyMontage(StartedInput.TargetReadyMontage);
}

void UUnitActionComponent::OnInputEnd_Implementation(const FInputClaim& EndedInput)
{
	if (UUnitMainComponent* CurrentUnit = GetOwnerUnit()) CurrentUnit->StopInputReadyMontage();
}