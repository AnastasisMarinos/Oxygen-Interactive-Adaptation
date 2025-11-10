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
		AutoFindAllTargets();
	}

	if (SnapshotColorTable.Num() == 0)
	{
		BuildDefaultSnapshotColors();
	}

	// Initialize current from first valid target (else white)
	if (ColorTargets.Num() > 0)
	{
		UObject* Obj = ColorTargets[0].Get();
		if (Obj && Obj->GetClass()->ImplementsInterface(ULightColorTarget::StaticClass()))
		{
			CurrentColor = ILightColorTarget::Execute_GetLightColor(Obj);
		}
	}
	StartColor = TargetColor = CurrentColor;

	// Push initial color
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

/* -------- Registration -------- */
void ALightSnapshotManager::RegisterColorTarget(UObject* Target)
{
	if (!Target) return;
	if (!Target->GetClass()->ImplementsInterface(ULightColorTarget::StaticClass()))
		return;

	// unique add
	for (const TWeakObjectPtr<UObject>& P : ColorTargets)
	{
		if (P.Get() == Target) return;
	}

	ColorTargets.Add(Target);
	// immediately push current color so it matches
	ILightColorTarget::Execute_SetLightColor(Target, CurrentColor);
}

void ALightSnapshotManager::UnregisterColorTarget(UObject* Target)
{
	ColorTargets.RemoveAllSwap([Target](const TWeakObjectPtr<UObject>& P)
	{
		return P.Get() == nullptr || P.Get() == Target;
	});
}

/* -------- API -------- */
void ALightSnapshotManager::ApplyLightColor(const FLinearColor& InTargetColor, float BlendSeconds)
{
	if (BlendSeconds <= 0.f) BlendSeconds = DefaultBlendSeconds;
	BeginBlendTo(InTargetColor, BlendSeconds);
}

void ALightSnapshotManager::ApplyLightSnapshot(EAudioSnapshot Snapshot, float BlendSeconds)
{
	if (const FLinearColor* Col = SnapshotColorTable.Find(Snapshot))
	{
		ApplyLightColor(*Col, BlendSeconds);
	}
	// no else/logs (as you asked to keep things clean)
}

/* -------- Internals -------- */
void ALightSnapshotManager::BeginBlendTo(const FLinearColor& InTarget, float InBlend)
{
	StartColor    = CurrentColor;
	TargetColor   = InTarget;
	BlendDuration = FMath::Max(0.01f, InBlend);
	BlendElapsed  = 0.f;
	bBlending     = true;

	if (BlendDuration <= 0.015f)
	{
		CurrentColor = TargetColor;
		PushColorAll(CurrentColor);
		bBlending = false;
	}
}

/** Simplified + safe: compact dead entries, then push to the rest */
void ALightSnapshotManager::PushColorAll(const FLinearColor& Color)
{
	ColorTargets.RemoveAllSwap([](const TWeakObjectPtr<UObject>& P)
	{
		UObject* Obj = P.Get();
		return (!Obj || !Obj->GetClass()->ImplementsInterface(ULightColorTarget::StaticClass()));
	});

	for (const TWeakObjectPtr<UObject>& P : ColorTargets)
	{
		if (UObject* Obj = P.Get())
		{
			ILightColorTarget::Execute_SetLightColor(Obj, Color);
		}
	}
}

/** Auto-discover anything that implements the interface */
void ALightSnapshotManager::AutoFindAllTargets()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), ULightColorTarget::StaticClass(), Found);
	for (AActor* A : Found)
	{
		RegisterColorTarget(A);
	}
}

/* Your existing palette; unchanged */
void ALightSnapshotManager::BuildDefaultSnapshotColors()
{
	auto C = [](uint8 R, uint8 G, uint8 B)
	{
		// keep your original desat or swap to FLinearColor::FromSRGBColor(FColor(R,G,B));
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