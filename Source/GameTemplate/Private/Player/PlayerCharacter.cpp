// (C) Anastasis Marinos 2025 //

#include "Player/PlayerCharacter.h"

#include "Blueprint/UserWidget.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/ItemTypes.h"
#include "UI/PlayerWidget.h"
#include "World/InteractableBase.h"


// Initialize Character & Set Default Values.
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Initialize Camera Arm & Set Default Values.
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraArm->SetupAttachment(GetMesh());
	CameraArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head_socket");
	CameraArm->SetRelativeLocation(FVector(0,-5,24));
	
	CameraArm->TargetArmLength = 0.0f;
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->bEnableCameraRotationLag = true;
	CameraArm->CameraRotationLagSpeed = 20.0f;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh());
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "weapon_socket");
	WeaponMesh->SetVisibility(false);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	Camera->AttachToComponent(CameraArm, FAttachmentTransformRules::KeepRelativeTransform);

	// Set Default Values For Character Capsule Component & Character Mesh.
	GetCapsuleComponent()->SetCapsuleRadius(26.0f);
	
	GetMesh()->SetRelativeLocation(FVector(0.0f,0.0f,-88.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f,-90.0f,0));

	// Set Default Values For Character Movement.
	GetCharacterMovement()->GravityScale = 1;
	GetCharacterMovement()->MaxAcceleration = 768;
	GetCharacterMovement()->BrakingFrictionFactor = 1.2;
	GetCharacterMovement()->GroundFriction = 7.5;
	GetCharacterMovement()->MaxWalkSpeed = 300;
	GetCharacterMovement()->BrakingDecelerationWalking = 256;
	GetCharacterMovement()->JumpZVelocity = 450;
	GetCharacterMovement()->AirControl = .4;
	GetCharacterMovement()->MaxFlySpeed = 150;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
}

// Called When The Game Starts Or When Spawned.
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);
		if (PlayerWidget)
		{
			PlayerWidget->AddToViewport();
		}
	}
}

void APlayerCharacter::Blink()
{
	if (PlayerWidget)
	{
		PlayerWidget->ToggleBlink();
	}
}

void APlayerCharacter::SetCurrentInteractable(AInteractableBase* Interactable)
{
	CurrentInteractable = Interactable;
}

void APlayerCharacter::Interact()
{
	if (bCanInteract)
	{
		bCanInteract = false;
		
		if (CurrentInteractable)
		{
			switch (CurrentInteractable->ItemType)
			{
			case EItemType::Headphones:
				
				break;
			
			case EItemType::Axe:
				bIsHoldingWeapon = true;
				WeaponMesh->SetVisibility(true);
				PickedUpWeapon();
				break;
			}
		
			CurrentInteractable->Interact();
			Blink();

			FTimerHandle InteractionCooldownDelay;
			GetWorldTimerManager().SetTimer(InteractionCooldownDelay, this, &APlayerCharacter::InteractionCooldown, 1.0f, false);
		}
	}
}

void APlayerCharacter::InteractionCooldown()
{
	bCanInteract = true;
}