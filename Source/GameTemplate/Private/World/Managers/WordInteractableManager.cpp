// © Anastasis Marinos //

#include "World/Managers/WordInteractableManager.h"
#include "World/Managers/AudioManager.h"
#include "World/Interactables/WordInteractable.h"
#include "World/Managers/LightSnapshotManager.h"
#include "Kismet/GameplayStatics.h"

AWordInteractableManager::AWordInteractableManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWordInteractableManager::BeginPlay()
{
	Super::BeginPlay();

	// Check if actor is set-up.
	checkf(AudioManager, TEXT("AudioManager is not set InteractableManager!"));
	checkf(AudioManager, TEXT("Interactable Class is not set Interactable Manager!"));

	// Bind Delegates.
	AudioManager->OnLineFinished.AddDynamic(this, &AWordInteractableManager::HandleLineFinished);

	// Pre-spawn line 1 interactable & light the scene.
	if (AudioManager->NarrationLines.IsValidIndex(0))
	{
		SpawnInteractable(AudioManager->NarrationLines[0].Keyword, RandomAnchorIndex(0));
		
		AudioManager->LightSnapshotManager->ApplyLightSnapshot(AudioManager->NarrationLines[0].Snapshot,0.0f);
	}
}

void AWordInteractableManager::HandleLineFinished(int32 LineIndex, const FNarrationLine&)
{
	// Spawn next interactable.
	const int32 NextIndex = LineIndex + 1;
	if (!AudioManager->NarrationLines.IsValidIndex(NextIndex))
		return; // nothing to spawn after the last line
	
	const FNarrationLine& Next = AudioManager->NarrationLines[NextIndex];
	const int32 AnchorIndex = RandomAnchorIndex(NextIndex);
	
	SpawnInteractable(Next.Keyword, AnchorIndex);
}

int32 AWordInteractableManager::RandomAnchorIndex(int32 NextLineIndex) const
{
	const int32 NumAnchors = SpawnAnchors.Num();

	// If a cycle order is provided, wrap by its length and map to an anchor index.
	if (CycleOrder.Num() > 0)
	{
		const int32 i = NextLineIndex % CycleOrder.Num();
		const int32 mapped = CycleOrder[i];
		return FMath::Clamp(mapped, 0, NumAnchors - 1);
	}

	return 0;
}

void AWordInteractableManager::SpawnInteractable(const FText& Keyword, int32 AnchorIndex)
{
	if (!SpawnAnchors.IsValidIndex(AnchorIndex)) return;

	AActor* Anchor = SpawnAnchors[AnchorIndex].Get();
	const FTransform SpawnLocation = Anchor->GetActorTransform();

	// Deferred spawn lets us set properties before BeginPlay runs on the interactable
	AWordInteractable* Station = GetWorld()->SpawnActorDeferred<AWordInteractable>(InteractableClass, SpawnLocation, this);
	Station->Keyword = Keyword;
	Station->AudioManager = AudioManager;

	UGameplayStatics::FinishSpawningActor(Station, SpawnLocation);
	ActiveStation = Station;
}