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

	// Make Interact widget initially hidden (your base already does this)
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
	// Play SFX and show widget logic are already in your base,
	// but we don't want to destroy immediately; so we DO NOT call Super::Interact().

	if (InteractionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());
	}

	// Fire BP hook if you want VFX/particles per-word
	OnInteract();

	// Trigger next narration line
	if (AudioManager)
	{
		AudioManager->PlayerTriggeredNextLine();
	}
	else if (bAutoFindManagers)
	{
		AutoFindManagers();
		if (AudioManager) AudioManager->PlayerTriggeredNextLine();
	}

	// Begin exit animation; actual Destroy() happens at the end of lowering
	StartLower();
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
	// Hide prompt while lowering
	if (InteractionWidget) InteractionWidget->SetVisibility(false);
	// Prevent further interaction
	if (InteractionSphere) InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWordInteractable::UpdateAnim(float DeltaSeconds)
{
	if (AnimState == EAnimState::Idle) return;

	const float Dur = (AnimState == EAnimState::Rising) ? FMath::Max(0.01f, RiseTime)
	                                                    : FMath::Max(0.01f, LowerTime);

	AnimT = FMath::Clamp(AnimT + DeltaSeconds / Dur, 0.f, 1.f);
	const float E = EaseInOutCubic(AnimT);

	if (AnimState == EAnimState::Rising)
	{
		// Move from BaseLoc - RiseDistance to BaseLoc
		const float Z = FMath::Lerp(BaseLoc.Z - RiseDistance, BaseLoc.Z, E);
		SetActorLocation(FVector(BaseLoc.X, BaseLoc.Y, Z));

		if (AnimT >= 1.f)
		{
			AnimState = EAnimState::Idle;
		}
	}
	else // Lowering
	{
		// Move from BaseLoc to BaseLoc - RiseDistance
		const float Z = FMath::Lerp(BaseLoc.Z, BaseLoc.Z - RiseDistance, E);
		SetActorLocation(FVector(BaseLoc.X, BaseLoc.Y, Z));

		if (AnimT >= 1.f)
		{
			Destroy();
		}
	}
}

void AWordInteractable::AutoFindManagers()
{
	UWorld* W = GetWorld();
	if (!W) return;

	if (!AudioManager)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(W, AAudioManager::StaticClass(), Found);
		if (Found.Num() > 0) AudioManager = Cast<AAudioManager>(Found[0]);
	}
}