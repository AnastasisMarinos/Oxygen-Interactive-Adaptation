// © Anastasis Marinos //

#include "World/Interactables/WordInteractable.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "World/Managers/AudioManager.h"

AWordInteractable::AWordInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Interaction sphere
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->InitSphereRadius(200.f);
	InteractionSphere->SetupAttachment(MeshComponent);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWordInteractable::BeginPlay()
{
	Super::BeginPlay();

	// Cache player
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Bind overlaps
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWordInteractable::OnSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AWordInteractable::OnSphereEndOverlap);

	// Rise from below
	BaseLoc = GetActorLocation();
	SetActorLocation(BaseLoc - FVector(0.f, 0.f, RiseDistance));
	StartRise();

	// Auto-find managers
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
	if (bInteracted)
		return;

	bInteracted = true;
	
	// VFX-only hook (don’t destroy inside)
	OnWordActivatedFX();

	// Trigger the next narration line
	if (!AudioManager && bAutoFindManagers)
	{
		AutoFindManagers();
	}
	if (AudioManager)
	{
		AudioManager->PlayerTriggeredNextLine();
	}

	// Start exit anim (Destroy() when finished lowering)
	StartLower();
}

/* ----------------- Overlap ----------------- */

void AWordInteractable::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerPawn)
	{
		// If your player stores a pointer, update it here:
		if (APlayerCharacter* PC = Cast<APlayerCharacter>(PlayerPawn))
		{
			PC->SetCurrentInteractable(this);
		}
	}
}

void AWordInteractable::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerPawn)
	{
		if (APlayerCharacter* PC = Cast<APlayerCharacter>(PlayerPawn))
		{
		    PC->SetCurrentInteractable(nullptr);
		}
	}
}

/* ----------------- Helpers ----------------- */

void AWordInteractable::StartRise()
{
	AnimState = EAnimState::Rising;
	AnimT = 0.f;
}

void AWordInteractable::StartLower()
{
	AnimState = EAnimState::Lowering;
	AnimT = 0.f;

	// Hide prompt & block further overlaps/interactions
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
		if (AnimT >= 1.f) AnimState = EAnimState::Idle;
	}
	else // Lowering
	{
		const float Z = FMath::Lerp(BaseLoc.Z, BaseLoc.Z - RiseDistance, E);
		SetActorLocation(FVector(BaseLoc.X, BaseLoc.Y, Z));
		if (AnimT >= 1.f) Destroy();
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