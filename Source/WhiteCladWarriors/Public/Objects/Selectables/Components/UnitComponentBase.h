// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitComponentBase.generated.h"

class UWidget;

USTRUCT(BlueprintType)
struct FGenericWidgetClaimer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	int UIOrder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FName ClaimWidgetType;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FName ClaimWidgetTag;
};

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WHITECLADWARRIORS_API UUnitComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	TArray<FGenericWidgetClaimer> InfoWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	TArray<FGenericWidgetClaimer> PortraitWidgets;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	void ConnectInfoWidget(UWidget* TargetWidget, FName ClaimedTag);
	virtual void ConnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	void DisconnectInfoWidget(UWidget* TargetWidget, FName ClaimedTag);
	virtual void DisconnectInfoWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	void ConnectPortraitWidget(UWidget* TargetWidget, FName ClaimedTag);
	virtual void ConnectPortraitWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	void DisconnectPortraitWidget(UWidget* TargetWidget, FName ClaimedTag);
	virtual void DisconnectPortraitWidget_Implementation(UWidget* TargetWidget, FName ClaimedTag) {}
//
//public:	
//	// Sets default values for this component's properties
//	UUnitComponentBase();
//
//protected:
//	// Called when the game starts
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//
//		
};
