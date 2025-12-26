// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerConnectable.h"
#include "Generals/Structs/InputPackage.h"
#include "GenericPlatform/ICursor.h"
#include "Operator.generated.h"

class AIngameController;
class AAreaSelector;
class AActionBase;
class UActionExecutor;
class UActionSelectorNode;
class AUnitBase;
class UCameraComponent;
class UUnitActionComponent;


UCLASS(BlueprintType)
class UActionTargetContainer : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TObjectPtr<AActionBase> Action;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	TArray<UUnitActionComponent*> Components;

public:
	bool operator < (const UActionTargetContainer& Other) const;
	bool operator > (const UActionTargetContainer& Other) const;
	
};

#define DEFAULT_CAMERALENGTH 2000.0f
#define DOUBLE_CLICK_DELAY 0.3
#define CLICK_CHECK_SQUARE_DISTANCE 800

DECLARE_DYNAMIC_DELEGATE_OneParam(FFunctionForSimpleAction, const FInputClaim&, Claim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedChanged, const TArray<AActor*>&, NewActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputClaimChanged, const FInputClaim&, NewClaim);


USTRUCT(BlueprintType)
struct FActionBinder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<AActionBase> Action;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TArray<UUnitActionComponent*> Components;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	FFunctionForSimpleAction Function;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	int Order;
};


UCLASS()
class WHITECLADWARRIORS_API AOperator : public APawn, public IPlayerConnectable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Select")
	FOnSelectedChanged OnSelectedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnInputClaimChanged OnInputClaimChanged;

protected:

	static TObjectPtr<AOperator> LocalOperator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess=true))
	TObjectPtr<UCameraComponent> SelectorCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess=true))
	TObjectPtr<AAreaSelector> DragAreaActor;

	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess=true))
	TObjectPtr<AIngameController> PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Select", meta = (AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> ClickAreaChannel;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Select", meta = (AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> SelectChannel;

	UPROPERTY(BlueprintReadOnly, Category = "Action", meta = (AllowPrivateAccess = true))
	TMap<FName, UActionTargetContainer*> AvailableActions;

	UPROPERTY(BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	FInputClaim CurrentInputClaim;

	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = true))
	FInputPackage CurrentInputPackage;

	UPROPERTY(BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	double LastLeftClickTime = -DOUBLE_CLICK_DELAY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	float CameraMovePaddingSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	float CameraLength = DEFAULT_CAMERALENGTH;

	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = true))
	bool bIsAdditiveMode;

	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = true))
	bool bIsSelectAllMode;

public:
	UFUNCTION(BlueprintPure, Category = "Camera")
	static bool IsVisibleOnCamera(FMatrix Matrix, AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Operator")
	static AOperator* GetLocalOperator() { return LocalOperator; }

public:
	void Tick(float DeltaSeconds) override;
	void PossessedBy(AController* NewController) override;
	void UnPossessed() override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void OnLeftClick(bool bIsMapClick, bool bIsAdditive, bool bIsSelectAll);
	virtual void OnLeftClick_Implementation(bool bIsMapClick, bool bIsAdditive, bool bIsSelectAll);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void OnRightClick(bool bIsMapClick);
	virtual void OnRightClick_Implementation(bool bIsMapClick);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void OnMapClick(bool bIsDown, bool bIsRightClick, FVector ClickLocation);
	virtual void OnMapClick_Implementation(bool bIsDown, bool bIsRightClick, FVector ClickLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void OnMapDrag(bool bIsRightClick, FVector ClickLocation);
	virtual void OnMapDrag_Implementation(bool bIsRightClick, FVector ClickLocation);

	UFUNCTION(BlueprintNativeEvent, Category = "Input")
	void OnUpdateInput();
	virtual void OnUpdateInput_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ClaimInput(AActionBase* ClaimAction, const FInputClaim& ClaimInfo);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ForceRemoveInputClaim();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void CancelInputClaim();

	UFUNCTION(BlueprintPure, Category = "Input")
	bool IsInputClaimed();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraMove(FVector2D Direction, float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraMoveTo(FVector Position);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void CameraZoom(float Value, float Min, float Max, float Multiplier);
	virtual void CameraZoom_Implementation(float Value, float Min, float Max, float Multiplier);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void SetCameraLength(float Value);
	virtual void SetCameraLength_Implementation(float Value);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void EdgeScroll(FVector2D MousePosition, FVector2D ViewportSize, float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ExecuteAction(AActionBase* TargetAction, const TArray<UUnitActionComponent*>& TargetComponent, bool bIsStartImmediately);

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<UActionTargetContainer*> GetAvailableActionList();

	UFUNCTION(BlueprintPure, Category = "Input")
	bool IsSmartKey(AActionBase* TargetAction) { return false; };

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

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	bool GetFocusActors(bool bIsClick, bool bIsDoubleClick, bool bIsSelectAll, TArray<AActor*>& OutResultArray, AActor*& OutResultSingle, bool& OutAllSame, bool& OutOnlySingle);
	virtual bool GetFocusActors_Implementation(bool bIsClick, bool bIsDoubleClick, bool bIsSelectAll, TArray<AActor*>& OutResultArray, AActor*& OutResultSingle, bool& OutAllSame, bool& OutOnlySingle);

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

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ComponentAddToActionList(UUnitActionComponent* Target);
	UFUNCTION(BlueprintCallable, Category = "Action")
	void ComponentRemoveFromActionList(UUnitActionComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ActorAddToActionList(AActor* Target);
	UFUNCTION(BlueprintCallable, Category = "Action")
	void ActorRemoveFromActionList(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SimpleAction(const FInputPackage& Input);

public:
	virtual void OnPlayerConnected_Implementation(AIngameController* NewPlayer);
	virtual void OnPlayerDisconnected_Implementation(AIngameController* OldPlayer);
	virtual AIngameController* GetConnectedPlayerController_Implementation() { return PlayerController; }
	
};
