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

	// Spawn next interactable when new line starts.
	UFUNCTION()
	void HandleLineStarted(int32 LineIndex, const FNarrationLine& Line);

	// Picks random anchor to spawn the next interactable.
	int32 RandomAnchorIndex(int32 NextLineIndex) const;

	// Spawn interactable at anchor index with the given keyword.
	void SpawnInteractable(const FText& Keyword, int32 AnchorIndex);

public:
	// Set to the audio manager in the level editor.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Refs")
	TObjectPtr<AAudioManager> AudioManager = nullptr;

	// Set to the interactable class in the actor editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TSubclassOf<AWordInteractable> InteractableClass;

	// Spawn anchors placed in the level.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Spawning")
	TArray<TObjectPtr<AActor>> SpawnAnchors;

	// Used to cycle between interactable spawn actors.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TArray<int32> CycleOrder = {0, 3, 1, 2};

private:
	mutable int32 CycleCursor = 0;
	
	TWeakObjectPtr<AWordInteractable> ActiveStation;
};