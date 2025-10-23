// (C) Anastasis Marinos 2025 //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AInteractableBase;
class UPlayerWidget;
class UWidgetComponent;

UCLASS()
class GAMETEMPLATE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// FUNCTIONS //
	APlayerCharacter();
	
	void Blink();
	
	void SetCurrentInteractable(AInteractableBase* Interactable);
	void Interact();

	UFUNCTION(BlueprintImplementableEvent)
	void PickedUpWeapon();

protected:
	// PROPERTIES & VARIABLES //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	class USpringArmComponent* CameraArm;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UMaterialInstance* WeaponBloodyMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* WeaponAttackMontage1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* WeaponAttackMontage2;

	UPROPERTY(BlueprintReadOnly, Category = "Items")
	bool bIsHoldingWeapon = false;

	UPROPERTY()
	AInteractableBase* CurrentInteractable;
	
	// FUNCTIONS //
	virtual void BeginPlay() override;

private:
	void InteractionCooldown();
	
	UPlayerWidget* PlayerWidget;
	
	FTimerHandle AttackCooldownHandle;

	bool bCanAttack = true;
	float AttackCooldown = 2.25f;

	float Health = 1.0f;

	bool bCanInteract = true;
};

