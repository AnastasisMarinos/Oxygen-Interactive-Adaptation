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
	// sRGB (0–255) → Linear + 20% desat toward luminance gray.
	auto C = [](uint8 R, uint8 G, uint8 B)
	{
		FLinearColor Lin = FLinearColor::FromSRGBColor(FColor(R, G, B));
		const float L = Lin.GetLuminance();
		const FLinearColor Gray(L, L, L);
		// 0.20 = ~20% less saturation
		return FMath::Lerp(Lin, Gray, 0.20f);
	};

	SnapshotColorTable.Empty();

	// --- Palette (base hex in comments) ---

	// TEAL (airy / celestial) 1
	SnapshotColorTable.Add(EAudioSnapshot::CELESTIAL,     C(34, 211, 238));  // #22D3EE  (teal-cyan)

	// ORANGE (earth) 1
	SnapshotColorTable.Add(EAudioSnapshot::TERRESTRIAL,   C(245, 158, 11));  // #F59E0B  (amber)

	// RED (conflict) 2 (deeper)
	SnapshotColorTable.Add(EAudioSnapshot::CONFLICT,      C(193, 18, 31));   // #C1121F  (deep red)

	// PURPLE (mourning) 2 (deeper violet)
	SnapshotColorTable.Add(EAudioSnapshot::MOURNING,      C(109, 40, 217));  // #6D28D9

	// ORANGE (family) 2 (burnt)
	SnapshotColorTable.Add(EAudioSnapshot::FAMILY,        C(217, 119, 6));   // #D97706

	// TEAL (science/crime) 2 (greener teal)
	SnapshotColorTable.Add(EAudioSnapshot::SCIENCE_CRIME, C(45, 212, 191));  // #2DD4BF

	// PURPLE (art) 1 (lavender)
	SnapshotColorTable.Add(EAudioSnapshot::ART,           C(167, 139, 250)); // #A78BFA

	// RED (vice) 1 (hot/coral red)
	SnapshotColorTable.Add(EAudioSnapshot::VICE,          C(242, 82, 92));   // #F2525C

	// BLUE (betrayal) 1 (icy/soft blue)
	SnapshotColorTable.Add(EAudioSnapshot::BETRAYAL,      C(96, 165, 250));  // #60A5FA

	// BLUE (politics) 2 (royal/civic blue)
	SnapshotColorTable.Add(EAudioSnapshot::POLITICS,      C(37, 99, 235));   // #2563EB

	// GREENISH-BLUE (reflection) (calm aqua/sea-green)
	SnapshotColorTable.Add(EAudioSnapshot::REFLECTION,    C(52, 211, 153));  // #34D399
}