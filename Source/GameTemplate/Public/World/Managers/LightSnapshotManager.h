// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/StageLight.h"
#include "World/Managers/AudioSnapshotManager.h"
#include "LightSnapshotManager.generated.h"

UCLASS()
class GAMETEMPLATE_API ALightSnapshotManager : public AActor
{
	GENERATED_BODY()

public:
	ALightSnapshotManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Auto-find AStageLight actors in level on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light")
	bool bAutoFindLights = true;

	/** Default blend time for calls that don't specify it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light")
	float DefaultBlendSeconds = 0.35f;

	/** Optional: color lookup per snapshot enum */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Snapshots")
	TMap<EAudioSnapshot, FLinearColor> SnapshotColorTable;

	/** Register/unregister fixtures (auto-done if bAutoFindLights = true) */
	UFUNCTION(BlueprintCallable, Category="Light")
	void RegisterLight(AStageLight* Light);

	UFUNCTION(BlueprintCallable, Category="Light")
	void UnregisterLight(AStageLight* Light);

	/** Apply raw color (lerp Using HSV) to all registered lights */
	UFUNCTION(BlueprintCallable, Category="Light")
	void ApplyLightColor(const FLinearColor& TargetColor, float BlendSeconds = -1.f);

	/** Apply color by snapshot enum (looks up in SnapshotColorTable) */
	UFUNCTION(BlueprintCallable, Category="Light")
	void ApplyLightSnapshot(EAudioSnapshot Snapshot, float BlendSeconds = -1.f);

private:
	TArray<TWeakObjectPtr<AStageLight>> Lights;

	// Blend state
	bool bBlending = false;
	float BlendElapsed = 0.f;
	float BlendDuration = 0.35f;

	FLinearColor StartColor = FLinearColor::White;
	FLinearColor TargetColor = FLinearColor::White;
	FLinearColor CurrentColor = FLinearColor::White;

	void BeginBlendTo(const FLinearColor& InTarget, float InBlend);
	void PushColorAll(const FLinearColor& Color);
	void AutoFindAllLights();
	void BuildDefaultSnapshotColors();
};