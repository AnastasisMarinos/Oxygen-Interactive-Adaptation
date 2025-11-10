// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordInteractableManager.generated.h"

struct FNarrationLine;
class AAudioManager;
class AWordInteractable;

UCLASS()
class GAMETEMPLATE_API AWordInteractableManager : public AActor
{
	GENERATED_BODY()

public:
	AWordInteractableManager();

protected:
	virtual void BeginPlay() override;

	/** When a narration line STARTS, we pre-spawn the station for the NEXT line */
	UFUNCTION()
	void HandleLineStarted(int32 LineIndex, const struct FNarrationLine& Line);

	/** Optional: fires when the whole sequence finishes */
	UFUNCTION()
	void HandleSequenceFinished();

	/** Spawns a station at anchor index with the given keyword */
	void SpawnStationFor(const FText& Keyword, int32 AnchorIndex);

	/** Picks which anchor to use for the NEXT line (uses ZoneIndex if valid, otherwise cycles) */
	int32 ResolveAnchorIndex(int32 NextLineIndex) const;

public:
	/** Reference to your AudioManager (auto-found if not set) */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Refs")
	TObjectPtr<AAudioManager> AudioManager = nullptr;

	/** The interactable class to spawn (e.g., BP_WordInteractable) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TSubclassOf<AWordInteractable> WordClass;

	/** Designated spawn anchors placed in the level (size 4+ as you like) */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Spawning")
	TArray<TObjectPtr<AActor>> SpawnAnchors;

	/** If true, use FNarrationLine.ZoneIndex when valid; otherwise use CycleOrder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	bool bRespectLineZoneIndex = true;

	/** Cycle order used when ZoneIndex is -1/invalid (indices into SpawnAnchors) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TArray<int32> CycleOrder = {0, 3, 1, 2};

	/**
	 * If true, spawn a station for line 0 immediately at BeginPlay.
	 * Enable only if you want a station visible before the first line starts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	bool bSpawnForFirstLineImmediately = false;

	/** If true, destroy/replace any existing station before spawning a new one */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	bool bReplaceOldStation = true;

private:
	/** Cursor for CycleOrder (mutable so ResolveAnchorIndex can advance it in a const context) */
	mutable int32 CycleCursor = 0;

	/** The currently spawned/active station (kept weak so GC isn’t held up) */
	TWeakObjectPtr<AWordInteractable> ActiveStation;

	/** Prevents accidental multiple AddDynamic bindings */
	bool bDelegatesBound = false;
};