// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Operator.generated.h"

#define DEFAULT_CAMERALENGTH 2000.0f

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedChanged, TArray<AActor*>, NewActors);

UCLASS()
class WHITECLADWARRIORS_API AOperator : public APawn
{
	GENERATED_BODY()

public:
	FOnSelectedChanged OnSelectedChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess=true))
	float CameraMovePaddingSize = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess=true))
	float CameraLength = DEFAULT_CAMERALENGTH;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess=true))
	TArray<AActor*> SelectedActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess=true))
	AActor* DragAreaActor;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess=true))
	AActor* MouseHitActor;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess=true))
	AActor* MouseClickActor;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess=true))
	FVector MouseTerrainPosition;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess=true))
	FVector DragStartPosition;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	TArray<AActor*> GetObjectsInArea();
	virtual TArray<AActor*> GetObjectsInArea_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	TArray<AActor*> GetVisibleSameObjects(AActor* Template);
	virtual TArray<AActor*> GetVisibleSameObjects_Implementation(AActor* Template);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DrawDragArea(FVector Begin, FVector End);
	virtual void DrawDragArea_Implementation(FVector Begin, FVector End);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectToggle(AActor* Target);
	virtual void SelectToggle_Implementation(AActor* Target);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectObjectWithoutNotify(AActor* Target, bool bIsSingleSelection);
	virtual void SelectObjectWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection);

	UFUNCTION(BlueprintCallable, Category = "Select")
	void SelectObject(AActor* Target, bool bIsSingleSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectObjects(const TArray<AActor*> Targets, bool bIsSingleSelection);
	virtual void SelectObjects_Implementation(const TArray<AActor*> Targets, bool bIsSingleSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectObjectWithoutNotify(AActor* Target);
	virtual void DeselectObjectWithoutNotify_Implementation(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Select")
	void DeselectObject(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectObjects();
	virtual void DeselectObjects_Implementation();
};
