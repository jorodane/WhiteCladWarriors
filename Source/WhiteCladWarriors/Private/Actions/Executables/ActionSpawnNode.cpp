// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionSpawnNode.h"
#include "Actions/Executables/ActionExecutor.h"

void UActionSpawnNode::InstanceRegistration_Implementation(AActor* Instance, const FActionCursorFinder& BaseCursor)
{
	if (!IsValid(Instance)) return;
	UActionExecutor* Executor = UActionExecutor::GetExecutorFromCursor(BaseCursor);
	if (!IsValid(Executor)) return;
	Executor->AddCreatedActor(Instance, this, BaseCursor);
}