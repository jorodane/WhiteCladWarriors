// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerConnectable.h"
#include "Operator.generated.h"

#define DEFAULT_CAMERALENGTH 2000.0f

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedChanged, const TArray<AActor*>&, NewActors);

class AIngameController;
class AAreaSelector;
class UCameraComponent;

UCLASS()
class WHITECLADWARRIORS_API AOperator : public APawn, public IPlayerConnectable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Select")
	FOnSelectedChanged OnSelectedChanged;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess=true))
	TObjectPtr<UCameraComponent> SelectorCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess=true))
	TObjectPtr<AAreaSelector> DragAreaActor;

	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess=true))
	TObjectPtr<AIngameController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess=true))
	TObjectPtr<AActor> MouseClickActor;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess=true))
	FVector MouseTerrainPosition;

	UPROPERTY(BlueprintReadWrite, Category = "Select", meta = (AllowPrivateAccess=true))
	FVector DragStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Select", meta = (AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> ClickAreaChannel;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Select", meta = (AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> SelectChannel;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess = true))
	TArray<AActor*> SelectedActors = TArray<AActor*>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	float CameraMovePaddingSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	float CameraLength = DEFAULT_CAMERALENGTH;

public:
	UFUNCTION(BlueprintPure, Category = "Camera")
	static bool IsVisibleOnCamera(FMatrix Matrix, AActor* Target);

public:
	void Tick(float DeltaSeconds) override;
	void PossessedBy(AController* NewController) override;
	void UnPossessed() override;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	TArray<AActor*> GetActorsInArea(bool& bIsAllSame, bool& bIsSingleSelected);
	virtual TArray<AActor*> GetActorsInArea_Implementation(bool& bIsAllSame, bool& bIsSingleSelected);


	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	TArray<AActor*> GetVisibleSameClasses(TSubclassOf<AActor> Template);
	virtual TArray<AActor*> GetVisibleSameClasses_Implementation(TSubclassOf<AActor> Template);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	TArray<AActor*> GetVisibleSameActors(AActor* Template);
	virtual TArray<AActor*> GetVisibleSameActors_Implementation(AActor* Template) { if (IsValid(Template)) return  GetVisibleSameClasses(Template->GetClass()); else return TArray<AActor*>(); }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	TArray<AActor*> GetOwnActors();
	virtual TArray<AActor*> GetOwnActors_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	TArray<AActor*> GetOwnActorsOfClass(TSubclassOf<AActor> Template);
	virtual TArray<AActor*> GetOwnActorsOfClass_Implementation(TSubclassOf<AActor> Template);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DrawDragArea(FVector Begin, FVector End);
	virtual void DrawDragArea_Implementation(FVector Begin, FVector End);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectToggle(AActor* Target);
	virtual void SelectToggle_Implementation(AActor* Target);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectActorWithoutNotify(AActor* Target, bool bIsSingleSelection);
	virtual void SelectActorWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection);

	UFUNCTION(BlueprintCallable, Category = "Select")
	void SelectActor(AActor* Target, bool bIsSingleSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectActors(const TArray<AActor*>& Targets, bool bIsSingleSelection);
	virtual void SelectActors_Implementation(const TArray<AActor*>& Targets, bool bIsSingleSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectActorWithoutNotify(AActor* Target);
	virtual void DeselectActorWithoutNotify_Implementation(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Select")
	void DeselectActor(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectActors();
	virtual void DeselectActors_Implementation();

public:
	void OnPlayerConnected_Implementation(AIngameController* NewPlayer);
	void OnPlayerDisconnected_Implementation(AIngameController* OldPlayer);
	AIngameController* GetConnectedPlayerController_Implementation() { return PlayerController; }

};
