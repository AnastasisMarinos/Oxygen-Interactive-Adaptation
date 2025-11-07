// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "WordInteractable.generated.h"

class UTextRenderComponent;
class ALightSnapshotManager;
class AAudioManager;

UCLASS()
class GAMETEMPLATE_API AWordInteractable : public AInteractableBase
{
	GENERATED_BODY()

public:
	AWordInteractable();

	// Keyword displayed on the totem (set per instance)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Word")
	FText Keyword;

	// Auto-find managers on BeginPlay if not set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	bool bAutoFindManagers = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TObjectPtr<AAudioManager> AudioManager = nullptr;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float RiseDistance = 300.f;     // cm down/up offset

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float RiseTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float LowerTime = 2.5f;

	// AInteractableBase override
	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// Spawn/Exit anim state
	enum class EAnimState : uint8 { Idle, Rising, Lowering };
	EAnimState AnimState = EAnimState::Idle;

	FVector BaseLoc;
	float   AnimT = 0.f; // 0..1 normalized progress

	// Helpers
	static float EaseInOutCubic(float T)
	{
		return (T < 0.5f) ? 4.f*T*T*T : 1.f - FMath::Pow(-2.f*T + 2.f, 3.f)/2.f;
	}

	void StartRise();
	void StartLower();
	void UpdateAnim(float DeltaSeconds);

	void AutoFindManagers();
};