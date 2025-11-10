// © Anastasis Marinos //

#include "World/Managers/WordInteractableManager.h"

#include "World/Managers/AudioManager.h"
#include "World/Interactables/WordInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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
	
	if (AudioManager && !bDelegatesBound)
	{
		AudioManager->OnLineStarted.AddDynamic(this, &AWordInteractableManager::HandleLineStarted);
		AudioManager->OnSequenceFinished.AddDynamic(this, &AWordInteractableManager::HandleSequenceFinished);
		bDelegatesBound = true;
	}

	// Optional: pre-spawn for line 0 BEFORE the first line starts (use with care)
	if (bSpawnForFirstLineImmediately && AudioManager && AudioManager->NarrationLines.IsValidIndex(0))
	{
		const int32 AnchorIdx = ResolveAnchorIndex(0);
		SpawnStationFor(AudioManager->NarrationLines[0].Keyword, AnchorIdx);
	}
}

void AWordInteractableManager::HandleLineStarted(int32 LineIndex, const FNarrationLine& /*Line*/)
{
	if (!AudioManager) return;

	// We spawn the station for the NEXT line
	const int32 NextIndex = LineIndex + 1;
	if (!AudioManager->NarrationLines.IsValidIndex(NextIndex))
	{
		return;
	}

	const FNarrationLine& Next = AudioManager->NarrationLines[NextIndex];

	// Optional: skip spawn if no keyword set
	if (Next.Keyword.ToString().TrimStartAndEnd().IsEmpty())
	{
		return;
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

	// Fallback: cycle through a custom order (or default to linear)
	if (CycleOrder.Num() > 0)
	{
		const int32 SafeCursor = (CycleCursor % CycleOrder.Num() + CycleOrder.Num()) % CycleOrder.Num();
		int32 AnchorIdx = CycleOrder[SafeCursor];

		// advance cursor (mutable so we can modify in const function)
		CycleCursor = (SafeCursor + 1) % CycleOrder.Num();

		return FMath::Clamp(AnchorIdx, 0, SpawnAnchors.Num() - 1);
	}

	// Default linear cycle
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
		// If your AWordInteractable has a graceful exit method, call it instead.
		ActiveStation->Destroy();
		ActiveStation.Reset();
	}

	const FTransform SpawnXf = Anchor->GetActorTransform();

	// Deferred spawn lets us set properties before BeginPlay/Construction logic runs
	AWordInteractable* Station = GetWorld()->SpawnActorDeferred<AWordInteractable>(WordClass, SpawnXf, this);
	if (!Station) return;

	Station->Keyword      = Keyword;
	Station->AudioManager = AudioManager;

	UGameplayStatics::FinishSpawningActor(Station, SpawnXf);

	ActiveStation = Station;
}