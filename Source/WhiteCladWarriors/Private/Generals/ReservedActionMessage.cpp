// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Generals/ReservedActionMessage.h"

const FName UReservedActionMessage::Canceled			= TEXT("Canceled");
const FName UReservedActionMessage::MoveSucceed			= TEXT("MoveSucceed");
const FName UReservedActionMessage::MoveFailed			= TEXT("MoveFailed");
const FName UReservedActionMessage::AnimationSucceed	= TEXT("AnimationSucceed");
const FName UReservedActionMessage::AnimationFailed		= TEXT("AnimationFailed");
const FName UReservedActionMessage::MontageNotifyBegin	= TEXT("MontageNotifyBegin");
const FName UReservedActionMessage::MontageNotifyEnd	= TEXT("MontageNotifyEnd");
const FName UReservedActionMessage::Die					= TEXT("Die");
const FName UReservedActionMessage::Revive				= TEXT("Revive");