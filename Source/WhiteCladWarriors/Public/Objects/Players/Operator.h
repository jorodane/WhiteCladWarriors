// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerConnectable.h"
#include "Generals/Structs/InputPackage.h"
#include "Generals/Structs/ActionInputStructures.h"
#include "Operator.generated.h"

class AIngameController;
class AAreaSelector;
class AActionBase;
class UActionExecutor;
class UActionSelectorNode;
class UActionIndicatorBase;
class UUnitActionComponent;
class UUnitMainComponent;
class UHeroMainComponent;
class UCameraComponent;



#define DEFAULT_CAMERALENGTH 2000.0f
#define DOUBLE_CLICK_DELAY 0.3
#define CLICK_CHECK_SQUARE_DISTANCE 800

DECLARE_DYNAMIC_DELEGATE_OneParam(FFunctionForSimpleAction, const FInputClaim&, Claim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedChanged, const TArray<AActor*>&, NewActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverChanged, UObject*, NewObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeroChanged, UHeroMainComponent*, NewHero);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInputClaimChanged, const FInputClaim&, NewClaim, bool, ValidClaim, bool, TriggerByIcon);


USTRUCT(BlueprintType)
struct FActionBinder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TObjectPtr<AActionBase> Action = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TArray<UUnitActionComponent*> Components;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	FFunctionForSimpleAction Function;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	int Order = 0;
};


UCLASS()
class WHITECLADWARRIORS_API AOperator : public APawn, public IPlayerConnectable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Select")
	FOnSelectedChanged OnSelectedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Select")
	FOnHoverChanged OnHoverChanged;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnHeroChanged OnHeroChanged;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnInputClaimChanged OnInputClaimChanged;

protected:

	static TObjectPtr<AOperator> LocalOperator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero", meta = (AllowPrivateAccess=true))
	TSubclassOf<AActor> HeroClass;

	UPROPERTY(BlueprintReadOnly, Category = "Hero", meta = (AllowPrivateAccess = true))
	TObjectPtr<UHeroMainComponent> HeroComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess=true))
	TObjectPtr<UCameraComponent> SelectorCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess=true))
	TObjectPtr<AAreaSelector> DragAreaActor;

	UPROPERTY(BlueprintReadOnly, Category = "Hero", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> HeroActor;
	UPROPERTY(BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> FocusActor;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess = true))
	TObjectPtr<UObject> HoveredOnWorldObject;

	UPROPERTY(BlueprintReadOnly, Category = "Select", meta = (AllowPrivateAccess = true))
	TObjectPtr<UObject> HoveredOnWidgetObject;

	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess=true))
	TObjectPtr<AIngameController> PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player", meta = (AllowPrivateAccess=true))
	TObjectPtr<UActionIndicatorBase> Indicator;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Select", meta = (AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> ClickAreaChannel;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Select", meta = (AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> SelectChannel;

	UPROPERTY(BlueprintReadOnly, Category = "Action", meta = (AllowPrivateAccess = true))
	TMap<FName, FActionTargetContainer> AvailableActions;

	UPROPERTY(BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	FInputClaim CurrentInputClaim;

	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = true))
	FInputPackage CurrentInputPackage;

	UPROPERTY(BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	FVector MouseScrolling = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	double LastLeftClickTime = -DOUBLE_CLICK_DELAY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	float CameraMovePaddingSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	float CameraLength = DEFAULT_CAMERALENGTH;

	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = true))
	bool bIsAdditiveMode = false;

	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = true))
	bool bIsSelectAllMode = false;

	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = true))
	bool bIsReservationMode = false;

public:
	UFUNCTION(BlueprintPure, Category = "Camera")
	static bool IsVisibleOnCamera(FMatrix Matrix, AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Operator")
	static AOperator* GetLocalOperator() { return LocalOperator; }

	UFUNCTION(BlueprintPure, Category = "Operator")
	static const FInputPackage& GetLocalInputPackage() { if (IsValid(LocalOperator)) return LocalOperator->CurrentInputPackage; else return FInputPackage::Input_None; }

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void OnUpdateCursor();
	virtual void OnUpdateCursor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	EInputMouseCursorType GetCursorType();
	virtual EInputMouseCursorType GetCursorType_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	EInputMouseCursorType GetCursorScrollType();
	virtual EInputMouseCursorType GetCursorScrollType_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ClaimInput(const FInputClaim& ClaimInfo);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ForceRemoveInputClaim();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void CancelInputClaim();

	UFUNCTION(BlueprintPure, Category = "Input")
	bool IsInputClaimed();

	UFUNCTION(BlueprintPure, Category = "Input")
	const FInputPackage& GetInputPackage() { return CurrentInputPackage; }

	UFUNCTION(BlueprintCallable, Category = "Input")
	void UpdateInputPackage();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraMove(FVector Direction, float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraMoveTo(FVector Position);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraMoveToFocusActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void CameraZoom(float Value, float Min, float Max, float Multiplier);
	virtual void CameraZoom_Implementation(float Value, float Min, float Max, float Multiplier);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void SetCameraLength(float Value);
	virtual void SetCameraLength_Implementation(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void ResetCameraRotation();
	virtual void ResetCameraRotation_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void EdgeScroll(FVector2D MousePosition, FVector2D ViewportSize, float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetFocusActor(AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Camera")
	AActor* GetFocusActor() { return FocusActor; }

	UFUNCTION(BlueprintPure, Category = "Camera")
	bool IsFocusHero();

	UFUNCTION(BlueprintPure, Category = "Select")
	bool IsOnlySelectHero();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SetHoveredWorldObject(UObject* NewObject);
	void SetHoveredWorldObject_Implementation(UObject* NewObject);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SetHoveredWidgetObject(UObject* NewObject);
	void SetHoveredWidgetObject_Implementation(UObject* NewObject);


	UFUNCTION(BlueprintCallable, Category = "Camera")
	inline void RemoveFocusActor();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetToggleFocusActor(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CommandAction(AActionBase* TargetAction, const TArray<UUnitActionComponent*>& TargetComponent, bool bIsStartImmediately);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void CommandActionForSelectedComponents(AActionBase* TargetAction, bool bIsStartImmediately);

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<FActionTargetContainer> GetAvailableActionList();

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<UUnitActionComponent*> GetAvailableComponentList(AActionBase* WantAction);

	UFUNCTION(BlueprintPure, Category = "Action")
	TArray<FActionTargetContainer> GetAvaliableActionFromKey(FKey WantKey) const;

	UFUNCTION(BlueprintPure, Category = "Input")
	bool IsSmartKey(AActionBase* TargetAction);

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
	void HideDragArea();
	virtual void HideDragArea_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Select")
	bool SelectTest(AActor* Target, bool bIsSingleSelected);
	bool SelectTest_Implementation(AActor* Target, bool bIsSingleSelected);

	UFUNCTION(BlueprintPure, Category = "Select")
	bool SelectInvalid(AActor* Target, bool bIsSingleSelected) { return !SelectTest(Target, bIsSingleSelected); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectToggle(AActor* Target);
	virtual void SelectToggle_Implementation(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectActorWithoutNotify(AActor* Target, bool bIsSingleSelection);
	virtual void SelectActorWithoutNotify_Implementation(AActor* Target, bool bIsSingleSelection);

	UFUNCTION(BlueprintCallable, Category = "Select")
	void SelectActor(AActor* Target, bool bIsSingleSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void SelectActors(TArray<AActor*>& Targets, bool bIsSingleSelection, bool bIsAdditionalSelection);
	virtual void SelectActors_Implementation(TArray<AActor*>& Targets, bool bIsSingleSelection, bool bIsAdditionalSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectActorWithoutNotify(AActor* Target);
	virtual void DeselectActorWithoutNotify_Implementation(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Select")
	void DeselectActor(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectUnit(UUnitMainComponent* Target);
	void DeselectUnit_Implementation(UUnitMainComponent* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectActors();
	virtual void DeselectActors_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void DeselectActorsWithoutNotify();
	virtual void DeselectActorsWithoutNotify_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Select")
	void BroadcastSelectChange();
	virtual void BroadcastSelectChange_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ComponentAddToActionList(UUnitActionComponent* Target);
	UFUNCTION(BlueprintCallable, Category = "Action")
	void ComponentRemoveFromActionList(UUnitActionComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool ActorAddToActionList(AActor* Target);
	UFUNCTION(BlueprintCallable, Category = "Action")
	void ActorRemoveFromActionList(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void SimpleAction(const FInputPackage& Input);

	UFUNCTION(BlueprintCallable, Category = "Action")
	AActionBase* GetSimpleActionFromActor(const FInputPackage& Input, AActor* Target, UUnitMainComponent*& OutUnit, TArray<UUnitActionComponent*>& OutComponents);

	UFUNCTION(BlueprintCallable, Category = "Action")
	AActionBase* GetSimpleActionFromComponent(const FInputPackage& Input, UUnitMainComponent* Target, TArray<UUnitActionComponent*>& OutComponents);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReservationAction(AActionBase* TargetAction, TArray<AActor*> TargetActors, const FInputPackage& Input, bool bIsStartImmediately);

	UFUNCTION(BlueprintCallable, Category = "Hero")
	UHeroMainComponent* SpawnHero(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Hero")
	void SetFollowingHero(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Hero")
	void ToggleFollowingHero();

public:
	virtual void OnPlayerConnected_Implementation(AIngameController* NewPlayer);
	virtual void OnPlayerDisconnected_Implementation(AIngameController* OldPlayer);
	virtual AIngameController* GetConnectedPlayerController_Implementation() { return PlayerController; }

public:
	UFUNCTION(BlueprintPure, Category = "Action")
	static TArray<FActionTargetContainer> GetAvailableActionListFromActors(const TArray<AActor*>& TargetArray);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void AppendAvailableActionFromComponent(UUnitActionComponent* TargetComponent, UPARAM(ref) TMap<FName, FActionTargetContainer>& OutMap);

	UFUNCTION(BlueprintCallable, Category = "Action")
	static void AppendAvailableActionFromActor(AActor* TargetActor, UPARAM(ref) TMap<FName, FActionTargetContainer>& OutMap);
	
};
