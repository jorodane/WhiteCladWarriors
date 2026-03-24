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
	float LocationOffset;
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Indicator")
	EInputIndicatorType GetIndicatorType();

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
	void SetHeight(float NewHeight);
	void SetHeight_Implementation(float NewHeight);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetLength(float NewLength);
	void SetLength_Implementation(float NewLength);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetWidthLength(float NewWidth, float NewLength);
	void SetWidthLength_Implementation(float NewWidth, float NewLength);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetPercent(float NewPercent);
	void SetPercent_Implementation(float NewPercent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetColor(FColor NewColor);
	void SetColor_Implementation(FColor NewColor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void SetFillColor(FColor NewColor);
	void SetFillColor_Implementation(FColor NewColor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawLineWithRotation(FVector StartPosition, FVector EndPosition, float Width, FRotator Rotation);
	void DrawLineWithRotation_Implementation(FVector StartPosition, FVector EndPosition, float Width, FRotator Rotation);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawWithRotation(FVector StartPosition, float Length, float Width, FRotator Rotation);
	void DrawWithRotation_Implementation(FVector StartPosition, float Length, float Width, FRotator Rotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawWithRotation(FVector StartPosition, FVector Direction, float Length, float Width);
	void DrawWithRotation_Implementation(FVector StartPosition, FVector Direction, float Length, float Width);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Indicator")
	void DrawLine(FVector StartPosition, FVector EndPosition, float Width);
	void DrawLine_Implementation(FVector StartPosition, FVector EndPosition, float Width);
};
