// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ActionBase.h"
#include "Actions/ActionSelectorNode.h"

bool AActionBase::IsRootNodeSelector(UActionSelectorNode*& AsSelectorNode) const
{
	AsSelectorNode = RootNodeAsSelector();
	return IsValid(AsSelectorNode);
}

bool AActionBase::IsNeedInputForStart(const FInputClaim& TriggerInput) const
{
	UActionSelectorNode* RootSelector;
	if (IsRootNodeSelector(RootSelector))
	{
		RootSelector->GetInputClaim();
		return true;
	}
	return false;
}


UActionSelectorNode* AActionBase::RootNodeAsSelector() const
{
	return Cast<UActionSelectorNode>(RootNode);
}