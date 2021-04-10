// Copyright Saad Khattak, All Rights Reserved.

#include "EnsureUtils.h"

#define LOCTEXT_NAMESPACE "EnsureUtils"

void UEnsureUtils::EnsureMsgf(bool InExpression, FText InMsg, EEnsureHitOrNot& OutIgnoreOrBreak, const UObject* InContext)
{
    if (InExpression == false)
    {
        OutIgnoreOrBreak = EEnsureHitOrNot::Hit;

        static const FText MessageTitle = LOCTEXT("EnsureMsgDialog", "Ignore and Continue?");

        const auto MessageAndCallStack = FString::Printf(TEXT("%s\n\n%s"), *InMsg.ToString(), *Get_StackTrace_Blueprint());

        switch(const auto& Result = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(MessageAndCallStack), &MessageTitle))
        {                                                                                                                                          
            case EAppReturnType::No:                                                                                                               
            {
                TryBreakInScript(InContext);                                                                                        
            }
            /* Handle the other case as you see fit */
            case EAppReturnType::Yes:                                                                                                               
            default: 
            {
            }
        }                                                                                                                                          
    }
    else
    {
        OutIgnoreOrBreak = EEnsureHitOrNot::NotHit;
    }

}

auto UEnsureUtils::TryBreakInScript(const UObject* InContext) -> void
{
#if UE_BUILD_DEVELOPMENT
    if (!IsValid(InContext))
    { return; }

    const auto* BlueprintExceptionTracker = FBlueprintContextTracker::TryGet();

    if (BlueprintExceptionTracker == nullptr)
    { return; }

    const TArrayView<FFrame* const>& RawStack = BlueprintExceptionTracker->GetCurrentScriptStackWritable();

    if (RawStack.IsEmpty())
    { return; }

    const auto TracepointExceptionInfo = FBlueprintExceptionInfo{EBlueprintExceptionType::Breakpoint};
    FBlueprintCoreDelegates::ThrowScriptException(InContext, *RawStack.Last(), TracepointExceptionInfo);
#endif
}

auto UEnsureUtils::Get_StackTrace_Blueprint() -> FString
{
    FString StackTrace;

#if UE_BUILD_DEVELOPMENT
    const auto* BlueprintExceptionTracker = FBlueprintContextTracker::TryGet();
    if (BlueprintExceptionTracker == nullptr)
    { return StackTrace; }

    const auto& RawStack = BlueprintExceptionTracker->GetCurrentScriptStack();
    for (auto FrameIdx = RawStack.Num() - 1; FrameIdx >= 0; --FrameIdx)
    {
        const auto& StackDescription = RawStack[FrameIdx];
        StackTrace += FString::Printf
        (
            TEXT("%s:%s\n"),
            *StackDescription->Node->GetName(),
            *StackDescription->MostRecentProperty->GetName()
        );
    }
#endif

    return StackTrace;
}

#undef LOCTEXT_NAMESPACE
