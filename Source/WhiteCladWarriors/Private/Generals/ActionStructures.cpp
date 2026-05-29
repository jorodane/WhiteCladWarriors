
#include "Generals/Structs/ActionStructures.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Actions/ActionBase.h"
#include "Actions/Executables/ActionExecutor.h"

const FActionCursorFinder FActionCursorFinder::None;
const FActionIntentContainer FActionIntentContainer::None;


bool FActionCursorFinder::CheckValid() const
{
	return IsValid(CurrentExecutor);
}

bool FActionCursorFinder::CheckExecutor(const UActionExecutor* WantExecutor) const
{
	if (!CheckValid()) return false;
	return CurrentExecutor == WantExecutor;
}
bool FActionCursorFinder::CheckOperator(const AOperator* WantOperator) const
{
	if(!CheckValid()) return false;
	return CurrentOperator == WantOperator;
}
bool FActionCursorFinder::CheckAction(const AActionBase* WantAction) const
{
	if(!CheckValid()) return false;
	return CurrentAction == WantAction;
}

void FActionCursorFinder::Set(AActionBase* WantAction, AOperator* WantOperator, UActionExecutor* WantExecutor, UUnitActionComponent* WantComponent, int WantID, bool bAsSubNode)
{
	CurrentAction = WantAction;
	CurrentOperator = WantOperator;
	CurrentExecutor = WantExecutor;
	CurrentComponent = WantComponent;
	CurrentID = WantID;
	bIsSubNode = bAsSubNode;
}


void FActionCursorFinder::Clear()
{
	CurrentAction = nullptr;
	CurrentOperator = nullptr;
	CurrentExecutor = nullptr;
	CurrentComponent = nullptr;
	CurrentID = 0;
	bIsSubNode = false;
}

int FActionTargetContainer::GetOrder() const
{
	return IsValid(this->Action) ? this->Action->GetUIOrder() : 0;
}

void FMainActionInfo::Clear()
{
	Cursor.Clear();
	Settings.Clear();
}

void FMainActionInfo::Clear(const UActionExecutor* OldExecutor)
{
	if (Cursor.CheckExecutor(OldExecutor)) Clear();
}
void FMainActionInfo::Set(const FActionCursorFinder& WantCursor, const FActionExecuteSettingContainer& WantSetting)
{
	Cursor = WantCursor;
	Settings = WantSetting;
}


void FMainActionInfo::SetActionMessage_Simple(FName Message)
{
	if (CheckValid()) Cursor.CurrentExecutor->SetActionMessage_Simple(Cursor, Message);
}

bool FMainActionInfo::Cancel()
{
	if (!CheckValid()) return true;
	if (!Settings.bIsCancelable) return false;

	if (UUnitActionComponent* TargetComponent = Cursor.CurrentComponent)
	{
		Cursor.CurrentExecutor->CancelNode(Cursor);
		UUnitMainComponent* TargetUnit;
		if (Settings.bIsStopActionMontageOnEnd && TargetComponent->TryGetOwnerUnit(TargetUnit))
		{
			TargetUnit->StopMainActionMontage(true);
		}
	}
	Clear();
	return true;
}

bool FMainActionInfo::End()
{
	if (CheckValid())
	{
		if (UUnitActionComponent* TargetComponent = Cursor.CurrentComponent)
		{
			if(IsValid(Cursor.CurrentExecutor)) Cursor.CurrentExecutor->CompleteNode(Cursor);
			TargetComponent->OnEndMainAction(Cursor.CurrentExecutor);
			UUnitMainComponent* TargetUnit;
			if (Settings.bIsStopActionMontageOnEnd && TargetComponent->TryGetOwnerUnit(TargetUnit))
			{
				TargetUnit->StopMainActionMontage(true);
			}
		}
		Clear();
		return true;
	}
	return false;
}


bool FMainActionInfo::CheckValid() const
{
	return Cursor.CheckValid();
}