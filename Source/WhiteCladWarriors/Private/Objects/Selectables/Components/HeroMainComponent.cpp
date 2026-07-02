// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Selectables/Components/HeroMainComponent.h"

UHeroMainComponent* UHeroMainComponent::GetHero(AActor* Target)
{
	if (IsValid(Target)) return Target->GetComponentByClass<UHeroMainComponent>();
	return nullptr;
}

bool UHeroMainComponent::TryGetHero(AActor* Target, UHeroMainComponent*& OutResult)
{
	OutResult = GetHero(Target);
	return OutResult != nullptr;
}