// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordInteractable.generated.h"

class USphereComponent;
class UWidgetComponent;
class UStaticMeshComponent;
class AAudioManager;

UCLASS()
class GAMETEMPLATE_API AWordInteractable : public AActor
{
	GENERATED_BODY()

public:
	AWordInteractable();

	// ---- Components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* InteractionSphere;

	// Animation //
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float RiseDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float RiseTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float LowerTime = 2.5f;

	// Content / Managers //
	UPROPERTY(BlueprintReadOnly, Category="Word")
	FText Keyword;

	UPROPERTY(BlueprintReadOnly, Category="Managers")
	AAudioManager* AudioManager = nullptr;

	// Interaction //
	void Interact();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	bool bInteracted = false;
	
	APawn* PlayerPawn = nullptr;

	// Animation //
	enum class EAnimState : uint8 { Idle, Rising, Lowering };
	EAnimState AnimState = EAnimState::Idle;
	FVector BaseLoc;
	float AnimT = 0.f;

	// Overlap //
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Animation Functions //
	void StartRise();
	void StartLower();
	void UpdateAnim(float DeltaSeconds);
};