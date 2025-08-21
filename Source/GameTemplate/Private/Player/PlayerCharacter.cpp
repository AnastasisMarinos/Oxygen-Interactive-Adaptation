// (C) Anastasis Marinos 2025 //

#include "Player/PlayerCharacter.h"

#include "AI/EnemyBase.h"
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

void APlayerCharacter::Attack()
{
	if (bIsHoldingWeapon)
	{
		if (!bCanAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack blocked due to cooldown."));
			// Attack is still on cooldown
			return;
		}

		bCanAttack = false; // Block further attacks

		AttackTrace();

		UAnimMontage* SelectedMontage = FMath::RandBool() ? WeaponAttackMontage1 : WeaponAttackMontage2;
		if (SelectedMontage && GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(SelectedMontage);
			UE_LOG(LogTemp, Warning, TEXT("Montage played"));
		}

		GetWorldTimerManager().SetTimer(AttackCooldownHandle, this, &APlayerCharacter::ResetAttackCooldown, AttackCooldown, false);
	}
}

void APlayerCharacter::AttackTrace()
{
	FVector Start = Camera->GetComponentLocation(); // or your weapon muzzle location
	FVector ForwardVector = Camera->GetForwardVector(); // or character facing direction
	FVector End = Start + ForwardVector * 200.f; // Trace distance (adjust as needed)

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignore self

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		APawn* HitPawn = Cast<APawn>(Hit.GetActor());
		if (HitPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit a pawn: %s"), *HitPawn->GetName());

			if (AEnemyBase* Enemy = Cast<AEnemyBase>(HitPawn))
			{
				if (bIsHoldingWeapon)
				{
					Enemy->ReceiveDamage(50.0f); // Adjust damage as needed
				}
				else
				{
					Enemy->ReceiveDamage(10.0f);
				}
			}
		}
	}

	// For debugging:
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);
}

void APlayerCharacter::ResetAttackCooldown()
{
	bCanAttack = true;

	if (WeaponBloodyMaterial)
	{
		WeaponMesh->SetMaterial(0, WeaponBloodyMaterial);
	}
}

void APlayerCharacter::RecieveDamage(float Damage)
{
	if (Health > 0.0f)
	{
		Health -= Damage;
	}
	else
	{
		// Die & Restart Level
	}
}
