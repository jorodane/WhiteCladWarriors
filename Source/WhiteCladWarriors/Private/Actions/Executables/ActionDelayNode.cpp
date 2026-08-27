// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Executables/ActionDelayNode.h"

void UActionDelayNode::StartTimer(const FActionCursorFinder& BaseCursor, float WantTime)
{
    if (WantTime <= 0.0f)
    {
        OnStartTimer(BaseCursor, WantTime);
        OnFinishTimer(BaseCursor);
        return;
    }
    if (TimerList.Contains(BaseCursor)) return;

    UWorld* World = GetWorld();
    if (!World)
    {
        OnBlockTimer(BaseCursor);
        return;
    }
    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    FActionCursorFinder ClaimCursor = BaseCursor;
    TimerDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UActionDelayNode, FinishTimer), BaseCursor);

    World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, WantTime, false);
    TimerList.Add(BaseCursor, TimerHandle);
    OnStartTimer(BaseCursor, WantTime);
}

void UActionDelayNode::CancelTimer(const FActionCursorFinder& BaseCursor)
{
    FTimerHandle* TimerHandle = TimerList.Find(BaseCursor);
    if (!TimerHandle) return;
    if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(*TimerHandle);
    TimerList.Remove(BaseCursor);
    OnCancelTimer(BaseCursor);
}

void UActionDelayNode::FinishTimer(const FActionCursorFinder& BaseCursor)
{
    if (TimerList.Remove(BaseCursor) <= 0) return;
    OnFinishTimer(BaseCursor);
}

void UActionDelayNode::ClearAll()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TPair<FActionCursorFinder, FTimerHandle>& Pair : TimerList) World->GetTimerManager().ClearTimer(Pair.Value);
    TimerList.Empty();
}