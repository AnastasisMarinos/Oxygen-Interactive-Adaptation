// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/Managers/AudioSnapshotManager.h"
#include "LightSnapshotManager.generated.h"

class AActor;

UCLASS()
class GAMETEMPLATE_API ALightSnapshotManager : public AActor
{
	GENERATED_BODY()

public:
	ALightSnapshotManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light")
	float DefaultBlendSeconds = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Snapshots")
	TMap<EAudioSnapshot, FLinearColor> SnapshotColorTable;

	// Register any Actor that implements ULightColorTarget.
	UFUNCTION(BlueprintCallable, Category="Light")
	void RegisterColorTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Light")
	void ApplyLightSnapshot(EAudioSnapshot Snapshot, float BlendSeconds = -1.f);

	UFUNCTION(BlueprintPure, Category="Light|Snapshots")
	FLinearColor GetCurrentColor() const { return CurrentColor; }

private:
	UPROPERTY(EditAnywhere, Category="Light")
	TArray<TObjectPtr<AActor>> LightTargets;

	// Blend state //
	bool bBlending = false;
	float BlendElapsed = 0.f;
	float BlendDuration = 0.35f;

	FLinearColor StartColor = FLinearColor::White;
	FLinearColor TargetColor = FLinearColor::White;
	FLinearColor CurrentColor = FLinearColor::White;
	
	void PushColorAll(const FLinearColor& Color);
	void AutoFindAllTargets();
	void BuildDefaultSnapshotColors();
};