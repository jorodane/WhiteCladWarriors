// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "Interfaces/Poolable.h"
#include "ActionIndicatorShowerBase.generated.h"

UCLASS()
class WHITECLADWARRIORS_API AActionIndicatorShowerBase : public AActor, public IPoolable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	FVector SizeMultiplier = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	float LocationOffset = 0.5f;

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Indicator")
	EInputIndicatorType GetIndicatorType();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Indicator")
	void SetPercent(float NewPercent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Indicator")
	void SetColor(FLinearColor NewColor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Indicator")
	void SetFillColor(FLinearColor NewColor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Indicator")
	void SetTexture(UTexture* NewTexture);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetMiddleLocation(FVector A, FVector B);
	void SetMiddleLocation_Implementation(FVector A, FVector B);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetStartLocation(FVector NewLocation);
	void SetStartLocation_Implementation(FVector NewLocation);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetDirection(FVector NewDirection);
	void SetDirection_Implementation(FVector NewDirection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetRotation(FRotator NewRotator);
	void SetRotation_Implementation(FRotator NewRotator);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetSize(FVector NewSize);
	void SetSize_Implementation(FVector NewSize);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetWidth(float NewWidth);
	void SetWidth_Implementation(float NewWidth);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetLength(float NewLength);
	void SetLength_Implementation(float NewLength);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetLengthWidth(float NewLength, float NewWidth);
	void SetLengthWidth_Implementation(float NewLength, float NewWidth);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawLineWithRotation(FVector StartPosition, FVector EndPosition, float NewWidth, FRotator NewRotation);
	void DrawLineWithRotation_Implementation(FVector StartPosition, FVector EndPosition, float NewWidth, FRotator NewRotation);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawFromRotation(FVector StartPosition, float NewLength, float NewWidth, FRotator NewRotation);
	void DrawFromRotation_Implementation(FVector StartPosition, float NewLength, float NewWidth, FRotator NewRotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawFromDirection(FVector StartPosition, FVector NewDirection, float NewLength, float NewWidth);
	void DrawFromDirection_Implementation(FVector StartPosition, FVector NewDirection, float NewLength, float NewWidth);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawLine(FVector StartPosition, FVector EndPosition, float NewWidth);
	void DrawLine_Implementation(FVector StartPosition, FVector EndPosition, float NewWidth);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawAt(FVector StartPosition, float NewWidth);
	void DrawAt_Implementation(FVector StartPosition, float NewRadius);
};
