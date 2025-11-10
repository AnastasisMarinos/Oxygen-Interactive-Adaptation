// © Anastasis Marinos //

#include "World/Interactables/WordInteractable.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "World/Managers/AudioManager.h"
#include "Engine/World.h"

AWordInteractable::AWordInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWordInteractable::BeginPlay()
{
	Super::BeginPlay();

	// Cache base location and start below ground for rise anim
	BaseLoc = GetActorLocation();
	SetActorLocation(BaseLoc - FVector(0.f, 0.f, RiseDistance));
	StartRise();

	// Auto-hook managers
	if (bAutoFindManagers)
	{
		AutoFindManagers();
	}
}

void AWordInteractable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateAnim(DeltaSeconds);
}

void AWordInteractable::Interact()
{
	if (bPlayerInRange)
	{
		if (bInteracted)
			return;

		// VFX-only hook (BP): NO Destroy here
		OnWordActivatedFX();

		bInteracted = true;

		// Trigger next narration line
		if (!AudioManager && bAutoFindManagers)
		{
			AutoFindManagers();
		}
		if (AudioManager)
		{
			AudioManager->PlayerTriggeredNextLine();
		}

		// Begin exit animation; actual Destroy() happens at the end of lowering
		StartLower();
	}
}

void AWordInteractable::StartRise()
{
	AnimState = EAnimState::Rising;
	AnimT = 0.f;
}

void AWordInteractable::StartLower()
{
	AnimState = EAnimState::Lowering;
	AnimT = 0.f;

	// Hide prompt while lowering & prevent further interaction
	if (InteractionWidget) InteractionWidget->SetVisibility(false);
	if (InteractionSphere) InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWordInteractable::UpdateAnim(float DeltaSeconds)
{
	if (AnimState == EAnimState::Idle) return;

	const float Dur = (AnimState == EAnimState::Rising)
		? FMath::Max(0.01f, RiseTime)
		: FMath::Max(0.01f, LowerTime);

	AnimT = FMath::Clamp(AnimT + DeltaSeconds / Dur, 0.f, 1.f);
	const float E = EaseInOutCubic(AnimT);

	if (AnimState == EAnimState::Rising)
	{
		const float Z = FMath::Lerp(BaseLoc.Z - RiseDistance, BaseLoc.Z, E);
		SetActorLocation(FVector(BaseLoc.X, BaseLoc.Y, Z));

		if (AnimT >= 1.f)
		{
			AnimState = EAnimState::Idle;
		}
	}
	else // Lowering
	{
		const float Z = FMath::Lerp(BaseLoc.Z, BaseLoc.Z - RiseDistance, E);
		SetActorLocation(FVector(BaseLoc.X, BaseLoc.Y, Z));

		if (AnimT >= 1.f)
		{
			Destroy(); // single, guaranteed destroy point
		}
	}
}

void AWordInteractable::AutoFindManagers()
{
	if (AudioManager) return;

	if (UWorld* W = GetWorld())
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(W, AAudioManager::StaticClass(), Found);
		if (Found.Num() > 0) AudioManager = Cast<AAudioManager>(Found[0]);
	}
}