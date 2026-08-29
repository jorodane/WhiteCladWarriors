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
    CreatedDelay.CurrentRepeatCount = 0;
    CreatedDelay.MaxRepeatCount = RepeatCount;
    CreatedDelay.Interval = WantTime;
    if (WantTime <= 0.0f)
    {
        OnStartTimer(BaseCursor);
        for (int i = 0; i < RepeatCount; ++i) ActivateTimerNative(BaseCursor, CreatedDelay);
        return;
    }
    FTimerDelegate& TimerDelegate = CreatedDelay.Delegate;
    TimerDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UActionDelayNode, ActivateTimer), BaseCursor);
    World->GetTimerManager().SetTimer(CreatedDelay.Handle, TimerDelegate, WantTime, RepeatCount > 1);
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
    FDelayInfo* DelayInfo = TimerList.Find(BaseCursor);
    if (!DelayInfo) return;
    FinishTimerNative(BaseCursor, *DelayInfo);
}

void UActionDelayNode::FinishTimerNative(const FActionCursorFinder& BaseCursor, FDelayInfo& Info)
{
    OnFinishTimer(BaseCursor);
    if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(Info.Handle);
    TimerList.Remove(BaseCursor);
}

void UActionDelayNode::ActivateTimer(const FActionCursorFinder& BaseCursor)
{
    FDelayInfo* DelayInfo = TimerList.Find(BaseCursor);
    if (!DelayInfo) return;
    ActivateTimerNative(BaseCursor, *DelayInfo);
}

void UActionDelayNode::ActivateTimerNative(const FActionCursorFinder& BaseCursor, FDelayInfo& Info)
{
    if (Info.CurrentRepeatCount <= Info.MaxRepeatCount)
    {
        OnActivated(BaseCursor, Info.CurrentRepeatCount, Info.MaxRepeatCount);
        ++Info.CurrentRepeatCount;
    }

    if(Info.CurrentRepeatCount >= Info.MaxRepeatCount) FinishTimerNative(BaseCursor, Info);
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