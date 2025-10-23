// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/PostProcessVolume.h"
#include "World/Managers/AudioSnapshotManager.h"
#include "PostProcessSnapshotManager.generated.h"

// Tunable targets we will interpolate toward
USTRUCT(BlueprintType)
struct FPostSnapshotTargets
{
	GENERATED_BODY()

	// 1.0 = neutral for Sat/Contrast; others are absolute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	float Saturation = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	float Contrast = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	float Vignette = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	float BloomIntensity = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	float BloomThreshold = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	float SceneFringe = 0.0f; // Chromatic aberration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	float Grain = 0.0f;
};

UCLASS()
class GAMETEMPLATE_API APostProcessSnapshotManager : public AActor
{
	GENERATED_BODY()

public:
	APostProcessSnapshotManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Assign the volume you want to drive (auto-finds an unbound one if null)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	TObjectPtr<APostProcessVolume> TargetVolume = nullptr;

	// Force Unbound on the target (useful for a single global look)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post")
	bool bForceUnbound = true;

	// Table of looks keyed by the same enum you use for audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Post|Snapshots")
	TMap<EAudioSnapshot, FPostSnapshotTargets> SnapshotTable;

	// Apply a look over BlendTimeSeconds
	UFUNCTION(BlueprintCallable, Category="Post|Snapshots")
	void ApplyPostSnapshot(EAudioSnapshot Snapshot, float BlendTimeSeconds = 0.35f);

private:
	// Interp state
	bool  bBlending = false;
	float BlendElapsed = 0.f;
	float BlendDuration = 0.35f;

	FPostSnapshotTargets Current;
	FPostSnapshotTargets Start;
	FPostSnapshotTargets Target;

	// Helpers
	void AutoFindTargetVolume();
	void SnapshotFromVolume(FPostSnapshotTargets& Out) const;
	void PushToVolume(const FPostSnapshotTargets& Values) const;
	void BeginBlendTo(const FPostSnapshotTargets& NewTarget, float InBlend);
	void BuildDefaultSnapshotTable();
};