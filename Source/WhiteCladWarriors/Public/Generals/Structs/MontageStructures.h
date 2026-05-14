// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MontageStructures.generated.h"

USTRUCT(BlueprintType)
struct FMontageEventInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimMontage> MontageToPlay = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TObjectPtr<AActor> FocusTarget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float PlayRate = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float StartingPosition = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float BlendInTime = 0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	float BlendOutTime = 0.2f;

	bool bIsStarted = false;

	void Clear();
	bool Play(UAnimInstance* Anim, bool bIsStopOtherMontage = true);
	void Stop(UAnimInstance* Anim);
};