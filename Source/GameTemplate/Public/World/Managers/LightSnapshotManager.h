// © Anastasis Marinos //
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/Lights/StageLight.h"
#include "World/Managers/AudioSnapshotManager.h"
#include "LightSnapshotManager.generated.h"

class AStageLight;

UCLASS()
class GAMETEMPLATE_API ALightSnapshotManager : public AActor
{
	GENERATED_BODY()

public:
	ALightSnapshotManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light")
	bool bAutoFindLights = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light")
	float DefaultBlendSeconds = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Snapshots")
	TMap<EAudioSnapshot, FLinearColor> SnapshotColorTable;

	/** Generic registration (any actor that implements ULightColorTarget) */
	UFUNCTION(BlueprintCallable, Category="Light")
	void RegisterColorTarget(UObject* Target);

	UFUNCTION(BlueprintCallable, Category="Light")
	void UnregisterColorTarget(UObject* Target);

	/** Back-compat helper */
	UFUNCTION(BlueprintCallable, Category="Light")
	void RegisterLight(AStageLight* Light) { RegisterColorTarget(Light); }

	UFUNCTION(BlueprintCallable, Category="Light")
	void UnregisterLight(AStageLight* Light) { UnregisterColorTarget(Light); }

	UFUNCTION(BlueprintCallable, Category="Light")
	void ApplyLightColor(const FLinearColor& TargetColor, float BlendSeconds = -1.f);

	UFUNCTION(BlueprintCallable, Category="Light")
	void ApplyLightSnapshot(EAudioSnapshot Snapshot, float BlendSeconds = -1.f);

	UFUNCTION(BlueprintPure, Category="Light|Snapshots")
	FLinearColor GetCurrentColor() const { return CurrentColor; }

private:
	/** Weak refs to any object implementing ULightColorTarget */
	TArray<TWeakObjectPtr<UObject>> ColorTargets;

	// Blend state
	bool bBlending = false;
	float BlendElapsed = 0.f;
	float BlendDuration = 0.35f;

	FLinearColor StartColor = FLinearColor::White;
	FLinearColor TargetColor = FLinearColor::White;
	FLinearColor CurrentColor = FLinearColor::White;

	void BeginBlendTo(const FLinearColor& InTarget, float InBlend);
	void PushColorAll(const FLinearColor& Color);
	void AutoFindAllTargets();
	void BuildDefaultSnapshotColors();
};