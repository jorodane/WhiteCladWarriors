// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionDelayNode.h"

void UActionDelayNode::StartTimer(const FActionCursorFinder& BaseCursor, float WantTime, int RepeatCount)
{
    if (RepeatCount <= 0) return;
    if (TimerList.Contains(BaseCursor)) return;
    UWorld* World = GetWorld();
    if (!World) 
    {
        OnBlockTimer(BaseCursor);
        return;
    }
    FDelayInfo& CreatedDelay = TimerList.Add(BaseCursor);
    CreatedDelay.CurrentRepeateCount = 0;
    CreatedDelay.MaxRepeatCount = RepeatCount;
    if (WantTime <= 0.0f)
    {
        OnStartTimer(BaseCursor);
        for (int i = 0; i < RepeatCount; i++) ActivateTimer(BaseCursor);
        OnFinishTimer(BaseCursor);
        return;
    }
    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    FActionCursorFinder ClaimCursor = BaseCursor;
    TimerDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UActionDelayNode, ActivateTimer), BaseCursor);

    World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, WantTime, false);
    CreatedDelay.Handle = TimerHandle;
    OnStartTimer(BaseCursor);
}

void UActionDelayNode::CancelTimer(const FActionCursorFinder& BaseCursor)
{
    FDelayInfo* DelayInfo = TimerList.Find(BaseCursor);
    if (!DelayInfo) return;
    FTimerHandle TimerHandle = DelayInfo->Handle;
    if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(TimerHandle);
    OnCancelTimer(BaseCursor);
    TimerList.Remove(BaseCursor);
}

void UActionDelayNode::FinishTimer(const FActionCursorFinder& BaseCursor)
{
    if (TimerList.Remove(BaseCursor) <= 0) return;
    OnFinishTimer(BaseCursor);
    TimerList.Remove(BaseCursor);
}

void UActionDelayNode::ActivateTimer(const FActionCursorFinder& BaseCursor)
{

}

void UActionDelayNode::ClearAll()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TPair<FActionCursorFinder, FDelayInfo>& Pair : TimerList)
    {
        World->GetTimerManager().ClearTimer(Pair.Value.Handle);
    }
    TimerList.Empty();
}