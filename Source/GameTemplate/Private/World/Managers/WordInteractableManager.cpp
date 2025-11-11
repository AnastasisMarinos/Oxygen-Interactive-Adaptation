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

	// Auto-find AudioManager if not set
	if (!AudioManager)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAudioManager::StaticClass(), Found);
		if (Found.Num() > 0)
		{
			AudioManager = Cast<AAudioManager>(Found[0]);
		}
	}

	// Bind delegates
	if (AudioManager && !bDelegatesBound)
	{
		AudioManager->OnLineStarted.AddDynamic(this, &AWordInteractableManager::HandleLineStarted);
		AudioManager->OnSequenceFinished.AddDynamic(this, &AWordInteractableManager::HandleSequenceFinished);
		bDelegatesBound = true;
	}

	// PRE-SPAWN for line 0 (since you no longer auto-start the first line)
	if (bSpawnForFirstLineImmediately && AudioManager && AudioManager->NarrationLines.IsValidIndex(0))
	{
		const int32 AnchorIdx = ResolveAnchorIndex(0);
		SpawnStationFor(AudioManager->NarrationLines[0].Keyword, AnchorIdx);

		// Optionally light the scene with the first line's snapshot immediately
		if (bApplyFirstLineSnapshotOnStart && AudioManager->LightSnapshotManager)
		{
			AudioManager->LightSnapshotManager->ApplyLightSnapshot(
				AudioManager->NarrationLines[0].Snapshot,
				0.0f /*instant*/
			);
		}
	}
}

void AWordInteractableManager::HandleLineStarted(int32 LineIndex, const FNarrationLine& /*Line*/)
{
	if (!AudioManager) return;

	// We spawn the station for the NEXT line
	const int32 NextIndex = LineIndex + 1;
	if (!AudioManager->NarrationLines.IsValidIndex(NextIndex))
	{
		return; // nothing to spawn after the last line
	}

	const FNarrationLine& Next = AudioManager->NarrationLines[NextIndex];
	if (Next.Keyword.ToString().TrimStartAndEnd().IsEmpty())
	{
		return; // optional: skip empty keywords
	}

	const int32 AnchorIndex = ResolveAnchorIndex(NextIndex);
	SpawnStationFor(Next.Keyword, AnchorIndex);
}

void AWordInteractableManager::HandleSequenceFinished()
{
	// End of sequence — remove any active station
	if (ActiveStation.IsValid())
	{
		ActiveStation->Destroy();
		ActiveStation.Reset();
	}
}

int32 AWordInteractableManager::ResolveAnchorIndex(int32 NextLineIndex) const
{
	if (!AudioManager || SpawnAnchors.Num() == 0) return 0;

	const FNarrationLine& Next = AudioManager->NarrationLines[NextLineIndex];

	// Respect authored ZoneIndex if requested
	if (bRespectLineZoneIndex)
	{
		if (Next.ZoneIndex >= 0 && Next.ZoneIndex < SpawnAnchors.Num())
		{
			return Next.ZoneIndex;
		}
	}

	// Fallback: cycle through custom order (or default linear)
	if (CycleOrder.Num() > 0)
	{
		const int32 SafeCursor = (CycleCursor % CycleOrder.Num() + CycleOrder.Num()) % CycleOrder.Num();
		int32 AnchorIdx = CycleOrder[SafeCursor];
		CycleCursor = (SafeCursor + 1) % CycleOrder.Num();
		return FMath::Clamp(AnchorIdx, 0, SpawnAnchors.Num() - 1);
	}

	return (NextLineIndex % SpawnAnchors.Num());
}

void AWordInteractableManager::SpawnStationFor(const FText& Keyword, int32 AnchorIndex)
{
	if (!WordClass) return;
	if (!SpawnAnchors.IsValidIndex(AnchorIndex)) return;

	AActor* Anchor = SpawnAnchors[AnchorIndex].Get();
	if (!Anchor) return;

	// Replace any existing station (prevents duplicates)
	if (bReplaceOldStation && ActiveStation.IsValid())
	{
		ActiveStation->Destroy();
		ActiveStation.Reset();
	}

	const FTransform SpawnXf = Anchor->GetActorTransform();

	// Deferred spawn lets us set properties before BeginPlay runs on the interactable
	AWordInteractable* Station = GetWorld()->SpawnActorDeferred<AWordInteractable>(WordClass, SpawnXf, this);
	if (!Station) return;

	Station->Keyword      = Keyword;
	Station->AudioManager = AudioManager;

	UGameplayStatics::FinishSpawningActor(Station, SpawnXf);
	ActiveStation = Station;
}