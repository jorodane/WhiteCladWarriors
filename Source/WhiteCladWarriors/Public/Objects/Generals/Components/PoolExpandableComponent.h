// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Objects/Generals/Components/PoolComponent.h"
#include "PoolExpandableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UPoolExpandableComponent : public UPoolComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool", meta = (ExposeOnSpawn = "true"))
	int CountOnExpand = 5;

protected:
	virtual AActor* OnWaitQueueEmpty_Implementation() override;
};
