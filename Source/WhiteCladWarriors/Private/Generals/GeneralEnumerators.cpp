#include "Generals/Structs/ActionStructures.h"
#include "Actions/ActionBase.h"


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