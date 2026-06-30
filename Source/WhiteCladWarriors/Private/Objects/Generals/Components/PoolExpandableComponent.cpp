// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Generals/Components/PoolExpandableComponent.h"
#include "Interfaces/Poolable.h"



AActor* UPoolExpandableComponent::OnWaitQueueEmpty_Implementation()
{
	SpawnWait(CountOnExpand);
	return SpawnLive();
}