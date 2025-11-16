// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Players/AreaSelector.h"
#include "Objects/Players/Operator.h"
#include "Interfaces/Selectable.h"

TArray<FHitResult> AAreaSelector::HitTraceInArea_Implementation() { return TArray<FHitResult>(); }


TArray<AActor*> AAreaSelector::GetActorsInArea_Implementation(AOperator* Operator, bool& bIsAllSame, bool& bIsSingleSelected)
{
	TArray<AActor*> Result;
	TSubclassOf<AActor> LastActorClass = nullptr;
	bIsAllSame = true;
	for (const FHitResult& CurrentHit : HitTraceInArea())
	{
		if (AActor* CurrentActor = CurrentHit.GetActor())
		{
			if (ISelectable::Execute_IsSelectable(CurrentActor, Operator))
			{
				Result.AddUnique(CurrentActor);
				if (bIsAllSame)
				{
					TSubclassOf<AActor> CurrentActorClass = CurrentActor->GetClass();
					if(LastActorClass != nullptr) bIsAllSame = LastActorClass == CurrentActorClass;
					else LastActorClass = CurrentActorClass;
				};
			}
		}
	}
	bIsSingleSelected = Result.Num() == 1;
	return Result;
}