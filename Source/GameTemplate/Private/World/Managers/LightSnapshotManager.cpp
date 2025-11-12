// © Anastasis Marinos //

#include "World/Managers/LightSnapshotManager.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/LightColorTarget.h"

ALightSnapshotManager::ALightSnapshotManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALightSnapshotManager::BeginPlay()
{
	Super::BeginPlay();

	AutoFindAllTargets();
	BuildDefaultSnapshotColors();
	
	StartColor = TargetColor = CurrentColor;
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

// ========================== [ Register Light Actors ] =============================== //

void ALightSnapshotManager::AutoFindAllTargets()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), ULightColorTarget::StaticClass(), Found);

	for (AActor* Actor : Found)
	{
		RegisterColorTarget(Actor);
	}
}

// Register any Actor that implements ULightColorTarget.
void ALightSnapshotManager::RegisterColorTarget(AActor* Target)
{
	if (!IsValid(Target)) return;
	if (!Target->GetClass()->ImplementsInterface(ULightColorTarget::StaticClass())) return;
	if (LightTargets.Contains(Target)) return;

	LightTargets.Add(Target);

	// Immediately push current color so it matches.
	ILightColorTarget::Execute_SetLightColor(Target, CurrentColor);
}

// ========================== [ Affect & Blend Lights ] =============================== //

void ALightSnapshotManager::ApplyLightSnapshot(EAudioSnapshot Snapshot, float BlendSeconds)
{
	if (const FLinearColor* InTargetColor = SnapshotColorTable.Find(Snapshot))
	{
		StartColor    = CurrentColor;
		TargetColor   = *InTargetColor;
		BlendDuration = FMath::Max(0.01f, BlendSeconds);
		BlendElapsed  = 0.f;
		bBlending     = true;

		if (BlendDuration <= 0.015f)
		{
			CurrentColor = TargetColor;
			PushColorAll(CurrentColor);
			bBlending = false;
		}
	}
}

void ALightSnapshotManager::PushColorAll(const FLinearColor& Color)
{
	// Trim destroyed actors.
	LightTargets.RemoveAllSwap([](AActor* Target){ return !IsValid(Target); });

	for (AActor* Target : LightTargets)
	{
		if (IsValid(Target) && Target->GetClass()->ImplementsInterface(ULightColorTarget::StaticClass()))
		{
			ILightColorTarget::Execute_SetLightColor(Target, Color);
		}
	}
}

// ========================== [ Snapshot Table ] =============================== //

void ALightSnapshotManager::BuildDefaultSnapshotColors()
{
	auto C = [](uint8 R, uint8 G, uint8 B)
	{
		FLinearColor Lin = FLinearColor::FromSRGBColor(FColor(R, G, B));
		const float L = Lin.GetLuminance();
		const FLinearColor Gray(L, L, L);
		return FMath::Lerp(Lin, Gray, 0.20f);
	};

	SnapshotColorTable.Empty();
	SnapshotColorTable.Add(EAudioSnapshot::CELESTIAL,     C(34, 211, 238));
	SnapshotColorTable.Add(EAudioSnapshot::TERRESTRIAL,   C(245, 158, 11));
	SnapshotColorTable.Add(EAudioSnapshot::CONFLICT,      C(193, 18, 31));
	SnapshotColorTable.Add(EAudioSnapshot::MOURNING,      C(109, 40, 217));
	SnapshotColorTable.Add(EAudioSnapshot::FAMILY,        C(217, 119, 6));
	SnapshotColorTable.Add(EAudioSnapshot::SCIENCE_CRIME, C(45, 212, 191));
	SnapshotColorTable.Add(EAudioSnapshot::ART,           C(167, 139, 250));
	SnapshotColorTable.Add(EAudioSnapshot::VICE,          C(242, 82, 92));
	SnapshotColorTable.Add(EAudioSnapshot::BETRAYAL,      C(96, 165, 250));
	SnapshotColorTable.Add(EAudioSnapshot::POLITICS,      C(37, 99, 235));
	SnapshotColorTable.Add(EAudioSnapshot::REFLECTION,    C(52, 211, 153));
}