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
	void HandleLineStarted(int32 LineIndex, const FNarrationLine& Line);

	UFUNCTION()
	void HandleSequenceFinished();

	/** Spawns a station at anchor index with the given keyword */
	void SpawnStationFor(const FText& Keyword, int32 AnchorIndex);

	/** Picks which anchor to use for the NEXT line (uses ZoneIndex if valid, otherwise cycles) */
	int32 ResolveAnchorIndex(int32 NextLineIndex) const;

public:
	/** AudioManager in the level (auto-found if not set) */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Refs")
	TObjectPtr<AAudioManager> AudioManager = nullptr;

	/** The interactable class to spawn (e.g., BP_WordInteractable) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TSubclassOf<AWordInteractable> WordClass;

	/** Designated spawn anchors placed in the level */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Spawning")
	TArray<TObjectPtr<AActor>> SpawnAnchors;

	/** Respect authored ZoneIndex when valid; else use CycleOrder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	bool bRespectLineZoneIndex = true;

	/** Cycle order used when ZoneIndex is -1/invalid (indices into SpawnAnchors) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TArray<int32> CycleOrder = {0, 3, 1, 2};

	/** Spawn a station for line 0 at BeginPlay (needed when you don't auto-start narration) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Startup")
	bool bSpawnForFirstLineImmediately = true;

	/** Also apply the first line's light snapshot at BeginPlay (so lights aren't dark) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Startup")
	bool bApplyFirstLineSnapshotOnStart = true;

	/** If true, destroy/replace any existing station before spawning a new one */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	bool bReplaceOldStation = true;

private:
	mutable int32 CycleCursor = 0;
	TWeakObjectPtr<AWordInteractable> ActiveStation;
	bool bDelegatesBound = false;
};