// (C) Anastasis Marinos 2025 //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AWordInteractable;
class UPlayerWidget;
class UWidgetComponent;

UCLASS()
class GAMETEMPLATE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// FUNCTIONS //
	APlayerCharacter();
	
	void SetCurrentInteractable(AWordInteractable* Interactable);
	void Interact();

	UFUNCTION(BlueprintCallable)
	void Blink();

protected:
	// PROPERTIES & VARIABLES //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	class USpringArmComponent* CameraArm;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerWidgetClass;

	UPROPERTY()
	AWordInteractable* CurrentInteractable;
	
	// FUNCTIONS //
	virtual void BeginPlay() override;

private:
	UPlayerWidget* PlayerWidget;
};

