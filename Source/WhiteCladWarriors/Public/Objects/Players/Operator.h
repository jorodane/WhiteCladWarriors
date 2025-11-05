// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Operator.generated.h"

UCLASS()
class WHITECLADWARRIORS_API AOperator : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess=true))
	float CameraMovePaddingSize = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess=true))
	float CameraLength = 1000.0f;

	const float DefaultCameraLength = 1000.0f;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraMove(FVector2D Direction, float Multiplier);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void CameraZoom(float Value, float Min, float Max, float Multiplier);
	void CameraZoom_Implementation(float Value, float Min, float Max, float Multiplier);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void SetCameraLength(float Value);
	void SetCameraLength_Implementation(float Value);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void EdgeScroll(FVector2D MousePosition, FVector2D ViewportSize, float Multiplier);
};
