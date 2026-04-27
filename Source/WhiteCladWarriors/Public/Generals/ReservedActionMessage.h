// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReservedActionMessage.generated.h"

UCLASS()
class WHITECLADWARRIORS_API UReservedActionMessage : public UObject
{
	GENERATED_BODY()

private:
	static const FName Canceled;
	static const FName MoveSucceed;
	static const FName MoveFailed;
	static const FName AnimationSucceed;
	static const FName AnimationFailed;
	static const FName MontageNotifyBegin;
	static const FName MontageNotifyEnd;
	static const FName Die;
	static const FName Revive;

public:
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_Canceled()			{ return Canceled; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_MoveSucceed()			{ return MoveSucceed; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_MoveFailed()			{ return MoveFailed; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_AnimationSucceed()	{ return AnimationSucceed; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_AnimationFailed()		{ return AnimationFailed; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_MontageNotifyBegin()	{ return MontageNotifyBegin; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_MontageNotifyEnd()	{ return MontageNotifyEnd; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_Die()					{ return Die; }
	UFUNCTION(BlueprintPure, Category = "Message")
	static FName GetActionMessage_Revive()				{ return Revive; }
};