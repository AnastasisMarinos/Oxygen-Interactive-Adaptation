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

	UPROPERTY()
	AInteractableBase* CurrentInteractable;
	
	// FUNCTIONS //
	virtual void BeginPlay() override;

private:
	UPlayerWidget* PlayerWidget;
};

