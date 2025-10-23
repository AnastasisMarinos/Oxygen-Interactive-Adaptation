// © Anastasis Marinos //

#include "World/Managers/PostProcessSnapshotManager.h"
#include "Kismet/GameplayStatics.h"

APostProcessSnapshotManager::APostProcessSnapshotManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APostProcessSnapshotManager::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetVolume)
	{
		AutoFindTargetVolume();
	}
	if (TargetVolume && bForceUnbound)
	{
		TargetVolume->bUnbound = true;
	}

	if (SnapshotTable.Num() == 0)
	{
		BuildDefaultSnapshotTable();
	}

	SnapshotFromVolume(Current);
	Start  = Current;
	Target = Current;
}

void APostProcessSnapshotManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bBlending || !TargetVolume) return;

	BlendElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(BlendElapsed / FMath::Max(BlendDuration, KINDA_SMALL_NUMBER), 0.f, 1.f);

	FPostSnapshotTargets Blended;
	Blended.Saturation     = FMath::Lerp(Start.Saturation,     Target.Saturation,     Alpha);
	Blended.Contrast       = FMath::Lerp(Start.Contrast,       Target.Contrast,       Alpha);
	Blended.Vignette       = FMath::Lerp(Start.Vignette,       Target.Vignette,       Alpha);
	Blended.BloomIntensity = FMath::Lerp(Start.BloomIntensity, Target.BloomIntensity, Alpha);
	Blended.BloomThreshold = FMath::Lerp(Start.BloomThreshold, Target.BloomThreshold, Alpha);
	Blended.SceneFringe    = FMath::Lerp(Start.SceneFringe,    Target.SceneFringe,    Alpha);
	Blended.Grain          = FMath::Lerp(Start.Grain,          Target.Grain,          Alpha);

	PushToVolume(Blended);

	if (Alpha >= 1.f)
	{
		bBlending = false;
		Current   = Target;
	}
}

void APostProcessSnapshotManager::ApplyPostSnapshot(EAudioSnapshot Snapshot, float BlendTimeSeconds)
{
	if (!SnapshotTable.Contains(Snapshot))
	{
		UE_LOG(LogTemp, Warning, TEXT("Post snapshot not found."));
		return;
	}
	BeginBlendTo(SnapshotTable[Snapshot], BlendTimeSeconds);
}

/* ---------------- Internals ---------------- */

void APostProcessSnapshotManager::AutoFindTargetVolume()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), Found);
	for (AActor* A : Found)
	{
		if (auto* PPV = Cast<APostProcessVolume>(A))
		{
			// Prefer an unbound volume if present
			if (PPV->bUnbound)
			{
				TargetVolume = PPV;
				return;
			}
		}
	}
	// Fallback: grab the first one
	if (Found.Num() > 0)
	{
		TargetVolume = Cast<APostProcessVolume>(Found[0]);
	}
}

void APostProcessSnapshotManager::SnapshotFromVolume(FPostSnapshotTargets& Out) const
{
	Out = FPostSnapshotTargets{};

	if (!TargetVolume) return;

	const FPostProcessSettings& S = TargetVolume->Settings;

	// Average the RGB channels to get a single scalar for Sat/Contrast
	Out.Saturation   = static_cast<float>((S.ColorSaturation.X + S.ColorSaturation.Y + S.ColorSaturation.Z) / 3.0);
	Out.Contrast     = static_cast<float>((S.ColorContrast.X   + S.ColorContrast.Y   + S.ColorContrast.Z)   / 3.0);
	Out.Vignette     = S.VignetteIntensity;
	Out.BloomIntensity = S.BloomIntensity;
	Out.BloomThreshold = S.BloomThreshold;
	Out.SceneFringe    = S.SceneFringeIntensity;
}

void APostProcessSnapshotManager::PushToVolume(const FPostSnapshotTargets& Vals) const
{
	if (!TargetVolume) return;

	FPostProcessSettings& S = TargetVolume->Settings;

	// Enable the overrides we touch
	S.bOverride_ColorSaturation      = true;
	S.bOverride_ColorContrast        = true;
	S.bOverride_VignetteIntensity    = true;
	S.bOverride_BloomIntensity       = true;
	S.bOverride_BloomThreshold       = true;
	S.bOverride_SceneFringeIntensity = true;

	// Set uniform RGB with alpha = 1
	S.ColorSaturation.X = Vals.Saturation;
	S.ColorSaturation.Y = Vals.Saturation;
	S.ColorSaturation.Z = Vals.Saturation;
	S.ColorSaturation.W = 1.0;

	S.ColorContrast.X = Vals.Contrast;
	S.ColorContrast.Y = Vals.Contrast;
	S.ColorContrast.Z = Vals.Contrast;
	S.ColorContrast.W = 1.0;

	S.VignetteIntensity    = Vals.Vignette;
	S.BloomIntensity       = Vals.BloomIntensity;
	S.BloomThreshold       = Vals.BloomThreshold;
	S.SceneFringeIntensity = Vals.SceneFringe;
}

void APostProcessSnapshotManager::BeginBlendTo(const FPostSnapshotTargets& NewTarget, float InBlend)
{
	Start         = Current;
	Target        = NewTarget;
	BlendDuration = FMath::Max(0.01f, InBlend);
	BlendElapsed  = 0.f;
	bBlending     = true;

	if (BlendDuration <= 0.015f)
	{
		PushToVolume(Target);
		Current = Target;
		bBlending = false;
	}
}

void APostProcessSnapshotManager::BuildDefaultSnapshotTable()
{
	FPostSnapshotTargets Cel; Cel.Saturation=1.05f; Cel.Contrast=0.95f; Cel.Vignette=0.10f; Cel.BloomIntensity=0.60f; Cel.BloomThreshold=0.80f; Cel.SceneFringe=0.00f; Cel.Grain=0.00f;
	SnapshotTable.Add(EAudioSnapshot::CELESTIAL, Cel);

	FPostSnapshotTargets Ter; Ter.Saturation=0.95f; Ter.Contrast=1.00f; Ter.Vignette=0.20f; Ter.BloomIntensity=0.20f; Ter.BloomThreshold=1.00f; Ter.SceneFringe=0.00f; Ter.Grain=0.05f;
	SnapshotTable.Add(EAudioSnapshot::TERRESTRIAL, Ter);

	FPostSnapshotTargets Con; Con.Saturation=1.00f; Con.Contrast=1.10f; Con.Vignette=0.30f; Con.BloomIntensity=0.10f; Con.BloomThreshold=1.20f; Con.SceneFringe=0.20f; Con.Grain=0.15f;
	SnapshotTable.Add(EAudioSnapshot::CONFLICT, Con);

	FPostSnapshotTargets Mou; Mou.Saturation=0.80f; Mou.Contrast=0.90f; Mou.Vignette=0.35f; Mou.BloomIntensity=0.40f; Mou.BloomThreshold=1.00f; Mou.SceneFringe=0.10f; Mou.Grain=0.20f;
	SnapshotTable.Add(EAudioSnapshot::MOURNING, Mou);

	FPostSnapshotTargets Fam; Fam.Saturation=1.05f; Fam.Contrast=1.00f; Fam.Vignette=0.15f; Fam.BloomIntensity=0.30f; Fam.BloomThreshold=0.95f; Fam.SceneFringe=0.00f; Fam.Grain=0.05f;
	SnapshotTable.Add(EAudioSnapshot::FAMILY, Fam);

	FPostSnapshotTargets Sci; Sci.Saturation=1.00f; Sci.Contrast=1.05f; Sci.Vignette=0.20f; Sci.BloomIntensity=0.20f; Sci.BloomThreshold=1.10f; Sci.SceneFringe=0.05f; Sci.Grain=0.05f;
	SnapshotTable.Add(EAudioSnapshot::SCIENCE_CRIME, Sci);

	FPostSnapshotTargets Art; Art.Saturation=1.10f; Art.Contrast=1.05f; Art.Vignette=0.12f; Art.BloomIntensity=0.70f; Art.BloomThreshold=0.85f; Art.SceneFringe=0.00f; Art.Grain=0.00f;
	SnapshotTable.Add(EAudioSnapshot::ART, Art);

	FPostSnapshotTargets Vic; Vic.Saturation=0.90f; Vic.Contrast=1.10f; Vic.Vignette=0.25f; Vic.BloomIntensity=0.15f; Vic.BloomThreshold=1.20f; Vic.SceneFringe=0.20f; Vic.Grain=0.25f;
	SnapshotTable.Add(EAudioSnapshot::VICE, Vic);

	FPostSnapshotTargets Bet; Bet.Saturation=0.95f; Bet.Contrast=1.00f; Bet.Vignette=0.30f; Bet.BloomIntensity=0.10f; Bet.BloomThreshold=1.10f; Bet.SceneFringe=0.10f; Bet.Grain=0.10f;
	SnapshotTable.Add(EAudioSnapshot::BETRAYAL, Bet);

	FPostSnapshotTargets Pol; Pol.Saturation=1.00f; Pol.Contrast=1.00f; Pol.Vignette=0.20f; Pol.BloomIntensity=0.20f; Pol.BloomThreshold=1.00f; Pol.SceneFringe=0.00f; Pol.Grain=0.05f;
	SnapshotTable.Add(EAudioSnapshot::POLITICS, Pol);

	FPostSnapshotTargets Ref; Ref.Saturation=0.85f; Ref.Contrast=0.95f; Ref.Vignette=0.33f; Ref.BloomIntensity=0.35f; Ref.BloomThreshold=1.05f; Ref.SceneFringe=0.05f; Ref.Grain=0.15f;
	SnapshotTable.Add(EAudioSnapshot::REFLECTION, Ref);
}