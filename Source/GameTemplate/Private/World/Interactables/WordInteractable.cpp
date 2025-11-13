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

	PromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidget"));
	PromptWidget->SetupAttachment(MeshComponent);
	PromptWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	PromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PromptWidget->SetDrawSize(FVector2D(220.f, 60.f));
	PromptWidget->SetVisibility(false); 
}

void AWordInteractable::BeginPlay()
{
	Super::BeginPlay();

	// Cache player.
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Bind overlaps.
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWordInteractable::OnSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AWordInteractable::OnSphereEndOverlap);

	// Apply widget to component.
	if (PromptWidget && PromptWidgetClass)
		PromptWidget->SetWidgetClass(PromptWidgetClass);
	
	// Rise from below.
	BaseLoc = GetActorLocation();
	SetActorLocation(BaseLoc - FVector(0.f, 0.f, RiseDistance));
	StartRise();
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

	if (PromptWidget)
		PromptWidget->SetVisibility(false);

	bInteracted = true;

	// Trigger the next narration line
	if (AudioManager)
	{
		AudioManager->PlayerTriggeredNextLine();
	}

	// Start exit anim (Destroy() when finished lowering)
	StartLower();
}

// ========================== [ Overlaps ] =============================== //

void AWordInteractable::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerPawn)
	{
		// If your player stores a pointer, update it here:
		if (APlayerCharacter* PC = Cast<APlayerCharacter>(PlayerPawn))
		{
			PC->SetCurrentInteractable(this);
			if (PromptWidget)
				PromptWidget->SetVisibility(true);
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
			if (PromptWidget)
				PromptWidget->SetVisibility(false);
		}
	}
}

// ========================== [ Animation ] =============================== //

void AWordInteractable::StartRise()
{
	AnimState = EAnimState::Rising;
	AnimT = 0.f;
}

void AWordInteractable::StartLower()
{
	AnimState = EAnimState::Lowering;
	AnimT = 0.f;
}

void AWordInteractable::UpdateAnim(float DeltaSeconds)
{
	if (AnimState == EAnimState::Idle) return;

	// Pick duration based on which leg we’re on.
	float dur = (AnimState == EAnimState::Rising)
		? FMath::Max(0.01f, RiseTime)
		: FMath::Max(0.01f, LowerTime);

	// Normalize progress 0..1
	AnimT = FMath::Clamp(AnimT + DeltaSeconds / dur, 0.f, 1.f);

	// From/To heights
	float fromZ = (AnimState == EAnimState::Rising) ? (BaseLoc.Z - RiseDistance) : BaseLoc.Z;
	float toZ   = (AnimState == EAnimState::Rising) ? BaseLoc.Z : (BaseLoc.Z - RiseDistance);

	// Smooth ease-in/out without custom math
	float t = FMath::InterpSinInOut(0.f, 1.f, AnimT);
	float z = FMath::Lerp(fromZ, toZ, t);
	SetActorLocation(FVector(BaseLoc.X, BaseLoc.Y, z));

	if (AnimT >= 1.f)
	{
		if (AnimState == EAnimState::Rising)
		{
			AnimState = EAnimState::Idle;
		}
		else // Lowering finished
		{
			Destroy();
		}
	}
}