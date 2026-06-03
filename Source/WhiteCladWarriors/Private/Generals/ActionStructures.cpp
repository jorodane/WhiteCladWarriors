
#include "Generals/Structs/ActionStructures.h"
#include "Objects/Selectables/Components/UnitActionComponent.h"
#include "Objects/Selectables/Components/UnitMainComponent.h"
#include "Actions/ActionBase.h"
#include "Actions/Executables/ActionExecutor.h"

const FActionCursorFinder FActionCursorFinder::None;
const FActionIntentContainer FActionIntentContainer::None;

UActionExecutor* FActionCursorFinder::GetExecutor() const
{
	return UActionExecutor::GetExecutorFromID(CurrentExecutorID);
}

bool FActionCursorFinder::CheckValid() const
{
	return IsValid(GetExecutor());
}

bool FActionCursorFinder::CheckExecutor(const int64 WantExecutorID) const
{
	if (!CheckValid()) return false;
	return CurrentExecutorID == WantExecutorID;
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

void FActionCursorFinder::Set(AActionBase* WantAction, AOperator* WantOperator, int64 WantExecutorID, UUnitActionComponent* WantComponent, int WantID, bool bWantSubNode)
{
	CurrentAction = WantAction;
	CurrentOperator = WantOperator;
	CurrentExecutorID = WantExecutorID;
	CurrentComponent = WantComponent;
	CurrentID = WantID;
	bAsSubNode = CurrentID != 0 || bWantSubNode;
}


void FActionCursorFinder::Clear()
{
	CurrentAction = nullptr;
	CurrentOperator = nullptr;
	CurrentExecutorID = -1;
	CurrentComponent = nullptr;
	CurrentID = 0;
	bAsSubNode = false;
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

void FMainActionInfo::Clear(int64 OldExecutorID)
{
	if (Cursor.CheckExecutor(OldExecutorID)) Clear();
}
void FMainActionInfo::Set(const FActionCursorFinder& WantCursor, const FActionExecuteSettingContainer& WantSetting)
{
	Cursor = WantCursor;
	Settings = WantSetting;
}


void FMainActionInfo::SetActionMessage_Simple(FName Message)
{
	UActionExecutor* Executor = Cursor.GetExecutor();
	if (CheckValid()) Executor->SetActionMessage_Simple(Cursor, Message);
}

bool FMainActionInfo::Cancel()
{
	if (!CheckValid()) return true;
	if (!Settings.bIsCancelable) return false;

	FActionCursorFinder CancelCursor = Cursor;
	Clear();
	UUnitActionComponent* TargetComponent = CancelCursor.CurrentComponent;
	if (IsValid(TargetComponent))
	{
		UActionExecutor* Executor = CancelCursor.GetExecutor();
		if(IsValid(Executor))Executor->CancelNode(CancelCursor);
	}
	return true;
}

bool FMainActionInfo::End()
{
	if (CheckValid())
	{
		FActionCursorFinder CancelCursor = Cursor;
		Clear();
		UUnitActionComponent* TargetComponent = CancelCursor.CurrentComponent;
		if (IsValid(TargetComponent))
		{
			UActionExecutor* Executor = CancelCursor.GetExecutor();
			if(IsValid(Executor))TargetComponent->OnEndMainAction(Executor->ExecutorID);
		}
		return true;
	}
	return false;
}


bool FMainActionInfo::CheckValid() const
{
	return Cursor.CheckValid();
}