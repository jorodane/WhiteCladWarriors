
#include "Generals/Structs/ActionStructures.h"
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

void FActionCursorFinder::Set(AActionBase* WantAction, AOperator* WantOperator, UActionExecutor* WantExecutor, UUnitActionComponent* WantComponent, int WantID)
{
	CurrentAction = WantAction;
	CurrentOperator = WantOperator;
	CurrentExecutor = WantExecutor;
	CurrentComponent = WantComponent;
	CurrentID = WantID;
}


void FActionCursorFinder::Clear()
{
	CurrentAction = nullptr;
	CurrentOperator = nullptr;
	CurrentExecutor = nullptr;
	CurrentComponent = nullptr;
	CurrentID = 0;
}

int FActionTargetContainer::GetOrder() const
{
	return IsValid(this->Action) ? this->Action->GetUIOrder() : 0;
}

