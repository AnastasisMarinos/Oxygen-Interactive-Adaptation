// © Anastasis Marinos //

#include "World/Managers/LightSnapshotManager.h"
#include "Kismet/GameplayStatics.h"

ALightSnapshotManager::ALightSnapshotManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALightSnapshotManager::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoFindLights)
	{
		AutoFindAllLights();
	}

	if (SnapshotColorTable.Num() == 0)
	{
		BuildDefaultSnapshotColors();
	}

	// Initialize current color from first light (if any), else white
	if (Lights.Num() > 0 && Lights[0].IsValid())
	{
		CurrentColor = Lights[0]->GetLightColor();
	}
	else
	{
		CurrentColor = FLinearColor::White;
	}
	StartColor = TargetColor = CurrentColor;

	// Ensure all lights are pushed to current color on start
	PushColorAll(CurrentColor);
}

void ALightSnapshotManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bBlending) return;

	BlendElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(BlendElapsed / FMath::Max(BlendDuration, KINDA_SMALL_NUMBER), 0.f, 1.f);

	CurrentColor = FLinearColor::LerpUsingHSV(StartColor, TargetColor, Alpha);
	PushColorAll(CurrentColor);

	if (Alpha >= 1.f)
	{
		bBlending = false;
	}
}

void ALightSnapshotManager::RegisterLight(AStageLight* Light)
{
	if (!Light) return;
	Lights.AddUnique(Light);
	Light->SetLightColor(CurrentColor);
}

void ALightSnapshotManager::UnregisterLight(AStageLight* Light)
{
	Lights.Remove(Light);
}

void ALightSnapshotManager::ApplyLightColor(const FLinearColor& InTargetColor, float BlendSeconds)
{
	if (BlendSeconds <= 0.f)
	{
		BlendSeconds = DefaultBlendSeconds;
	}
	BeginBlendTo(InTargetColor, BlendSeconds);
}

void ALightSnapshotManager::ApplyLightSnapshot(EAudioSnapshot Snapshot, float BlendSeconds)
{
	if (!SnapshotColorTable.Contains(Snapshot))
	{
		UE_LOG(LogTemp, Warning, TEXT("LightSnapshotManager: No color for snapshot."));
		return;
	}
	ApplyLightColor(SnapshotColorTable[Snapshot], BlendSeconds);
}

void ALightSnapshotManager::BeginBlendTo(const FLinearColor& InTarget, float InBlend)
{
	StartColor   = CurrentColor;
	TargetColor  = InTarget;
	BlendDuration= FMath::Max(0.01f, InBlend);
	BlendElapsed = 0.f;
	bBlending    = true;

	if (BlendDuration <= 0.015f)
	{
		CurrentColor = TargetColor;
		PushColorAll(CurrentColor);
		bBlending = false;
	}
}

void ALightSnapshotManager::PushColorAll(const FLinearColor& Color)
{
	for (int32 i = Lights.Num() - 1; i >= 0; --i)
	{
		AStageLight* L = Lights[i].Get();
		if (!L)
		{
			Lights.RemoveAtSwap(i);
			continue;
		}
		L->SetLightColor(Color);
	}
}

void ALightSnapshotManager::AutoFindAllLights()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStageLight::StaticClass(), Found);
	for (AActor* A : Found)
	{
		RegisterLight(Cast<AStageLight>(A));
	}
}

void ALightSnapshotManager::BuildDefaultSnapshotColors()
{
	// Gentle defaults matching your snapshots; tweak in Details panel later
	SnapshotColorTable.Add(EAudioSnapshot::CELESTIAL,       FLinearColor(0.85f, 0.90f, 1.00f)); // cool airy white
	SnapshotColorTable.Add(EAudioSnapshot::TERRESTRIAL,     FLinearColor(1.00f, 0.90f, 0.70f)); // warm amber
	SnapshotColorTable.Add(EAudioSnapshot::CONFLICT,        FLinearColor::White);               // tight white
	SnapshotColorTable.Add(EAudioSnapshot::MOURNING,        FLinearColor(0.75f, 0.75f, 0.80f)); // desat
	SnapshotColorTable.Add(EAudioSnapshot::FAMILY,          FLinearColor(1.00f, 0.92f, 0.80f)); // soft amber
	SnapshotColorTable.Add(EAudioSnapshot::SCIENCE_CRIME,   FLinearColor(0.70f, 0.90f, 1.00f)); // cool lab
	SnapshotColorTable.Add(EAudioSnapshot::ART,             FLinearColor(1.00f, 0.98f, 0.92f)); // elegant warm white
	SnapshotColorTable.Add(EAudioSnapshot::VICE,            FLinearColor(0.90f, 0.70f, 0.90f)); // dimmed magenta
	SnapshotColorTable.Add(EAudioSnapshot::BETRAYAL,        FLinearColor(0.85f, 0.90f, 1.00f)); // chilly white
	SnapshotColorTable.Add(EAudioSnapshot::POLITICS,        FLinearColor(1.00f, 1.00f, 1.00f)); // neutral
	SnapshotColorTable.Add(EAudioSnapshot::REFLECTION,      FLinearColor(0.90f, 0.95f, 1.00f)); // quiet cool
}