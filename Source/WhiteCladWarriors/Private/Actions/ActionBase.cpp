// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionBase.h"
#include "Actions/ActionSelectorNode.h"

bool AActionBase::IsRootNodeSelector() const
{
	return IsValid(Cast<UActionSelectorNode>(RootNode));
}
