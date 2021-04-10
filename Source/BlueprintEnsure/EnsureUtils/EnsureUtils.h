// Copyright Saad Khattak, All Rights Reserved.

#pragma once
#include "Misc/MessageDialog.h"

#include "EnsureUtils.generated.h"

/*
 * Notes: This is a minimal example (and thus not the 'best' code) to show how one can force a break in
 * BP which can be very useful for BP Ensures. Ideally, the message would include the BP callstack
 */

UENUM(BlueprintType)
enum class EEnsureHitOrNot : uint8
{
    NotHit,
    Hit
};

UCLASS()
class BLUEPRINTENSURE_API UEnsureUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs = "OutIgnoreOrBreak", DefaultToSelf="InContext", HidePin="InContext"))
    static void EnsureMsgf(bool InExpression, FText InMsg, EEnsureHitOrNot& OutIgnoreOrBreak, const UObject* InContext = nullptr);

public:
    static auto TryBreakInScript(const UObject* InContext) -> void;
    static auto Get_StackTrace_Blueprint() -> FString;
};
