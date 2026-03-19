// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Generals/Components/PoolComponent.h"


// Called when the game starts
void UPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UPoolComponent::BeginDestroy()
{
	Super::BeginDestroy();
}

void UPoolComponent::SpawnWait_Implementation(int Count)
{
	
}

TArray<AActor*> UPoolComponent::SpawnInstance_Implementation(int Count)
{

}

AActor* UPoolComponent::CreateInstance_Implementation()
{

}

void UPoolComponent::DestroyInstance_Implementation(AActor* Target)
{

}


