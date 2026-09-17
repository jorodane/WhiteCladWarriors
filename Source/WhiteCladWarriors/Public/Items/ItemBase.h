#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemBase.generated.h"

class UItemInstanceBase;
class UInventoryBase;

UCLASS(BlueprintType, Blueprintable)
class WHITECLADWARRIORS_API UItemBase : public UObject
{
    GENERATED_BODY()
public:
    UItemBase();

    // Configuration phase only. First registration/creation permanently seals this object.
    UFUNCTION(BlueprintCallable, Category="Item")
    bool InitializeDefinition(FName InItemId, int32 InSlotMax, int32 InInventoryMax,
        TSubclassOf<UItemInstanceBase> InInstanceClass);

    // Detached instance. The caller must keep a strong reference until adoption.
    UFUNCTION(BlueprintCallable, Category="Item")
    UItemInstanceBase* CreateItemInstance(int32 Amount = 0);

    UFUNCTION(BlueprintPure, Category="Item")
    bool IsDefinitionSealed() const { return bDefinitionSealed; }

    UFUNCTION(BlueprintPure, Category="Item")
    FName GetItemId() const { return ItemId; }
    UFUNCTION(BlueprintPure, Category="Item")
    int32 GetMaxStackEachSlot() const { return MaxStackEachSlot; }

    const FText& GetDisplayName() const { return DisplayName; }
    const FText& GetDisplayContext() const { return DisplayContext; }

    // Snapshot: safe to iterate even when replacement unregisters instances.
    UFUNCTION(BlueprintCallable, Category="Item")
    TArray<UItemInstanceBase*> GetLiveInstances();

    // Only committed, inventory-owned instances. Not a market-availability metric.
    UFUNCTION(BlueprintPure, Category="Item")
    int64 GetOwnedTotal() const;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
    FName ItemId = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
    FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
    FText DisplayContext;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(ClampMin="1", AllowPrivateAccess="true"))
    int32 MaxStackEachSlot = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemInstanceBase> InstanceClass;

private:
    friend class UInventoryBase;
    bool SealDefinition();
    void UnregisterInstance(UItemInstanceBase* Instance);

    UPROPERTY(Transient)
    bool bDefinitionSealed = false;

    UPROPERTY(Transient)
    TArray<TWeakObjectPtr<UItemInstanceBase>> InstancedItems;
};
