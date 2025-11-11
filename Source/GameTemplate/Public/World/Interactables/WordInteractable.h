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
	virtual void Tick(float DeltaSeconds) override;

	/** Called by the player when pressing Interact */
	UFUNCTION(BlueprintCallable, Category="Word")
	void Interact();

	/** VFX/SFX hook – do NOT Destroy in here */
	UFUNCTION(BlueprintImplementableEvent, Category="Word")
	void OnWordActivatedFX();

	// ---- Content / Managers ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Word")
	FText Keyword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	bool bAutoFindManagers = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TObjectPtr<AAudioManager> AudioManager = nullptr;

	// ---- Animation ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float RiseDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float RiseTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float LowerTime = 2.5f;

	// ---- Components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* InteractionSphere;

protected:
	virtual void BeginPlay() override;

private:
	// One-shot safety
	bool bInteracted = false;

	// Player proximity
	APawn* PlayerPawn = nullptr;

	// Anim state
	enum class EAnimState : uint8 { Idle, Rising, Lowering };
	EAnimState AnimState = EAnimState::Idle;
	FVector BaseLoc;
	float   AnimT = 0.f; // 0..1

	// Easing (kept the nice cubic ease-in-out)
	static float EaseInOutCubic(float T)
	{
		return (T < 0.5f) ? 4.f*T*T*T : 1.f - FMath::Pow(-2.f*T + 2.f, 3.f)/2.f;
	}

	// Overlap
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Helpers
	void StartRise();
	void StartLower();
	void UpdateAnim(float DeltaSeconds);
	void AutoFindManagers();
};