#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

class UItemInstanceBase;
class UInventorySlot;

UENUM(BlueprintType)
enum class EInventoryResult : uint8
{
    Success,
    NoChange,
    WrongThread,
    Busy,
    InvalidArgument,
    InvalidDefinition,
    UnknownItemId,
    NotOwned,
    AlreadyOwned,
    Retired,
    Occupied,
    InsufficientQuantity,
    InventoryLimitReached,
    StackLimitReached,
    CapacityExceeded,
    QuantityOverflow,
    IncompatibleItems,
    StateCopyRejected,
    StateContractViolation
};

inline bool InventorySucceeded(EInventoryResult Result)
{
    return Result == EInventoryResult::Success || Result == EInventoryResult::NoChange;
}

// Old/New snapshots make a UI update independent of delegate listener order.
USTRUCT(BlueprintType)
struct WHITECLADWARRIORS_API FInventorySlotChange
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UInventorySlot> Slot = nullptr;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UItemInstanceBase> OldItem = nullptr;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UItemInstanceBase> NewItem = nullptr;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 OldPosition = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 NewPosition = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 OldAmount = 0;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 NewAmount = 0;
};

USTRUCT(BlueprintType)
struct WHITECLADWARRIORS_API FInventoryItemChange
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UItemInstanceBase> Item = nullptr;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 OldCount = 0;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 NewCount = 0;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    bool bWasOwned = false;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    bool bIsOwned = false;
};

USTRUCT(BlueprintType)
struct WHITECLADWARRIORS_API FInventoryInstanceReplacement
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UItemInstanceBase> OldItem = nullptr;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UItemInstanceBase> NewItem = nullptr;
};

USTRUCT(BlueprintType)
struct WHITECLADWARRIORS_API FInventoryChangeSet
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TArray<FInventorySlotChange> Slots;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TArray<FInventoryItemChange> Items;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TArray<FInventoryInstanceReplacement> Replacements;

    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    bool bCapacityChanged = false;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 OldCapacity = 0;
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    int32 NewCapacity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnInventoryChanged, const FInventoryChangeSet&, Changes);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryChangedNative, const FInventoryChangeSet&);
