// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "StageLight.generated.h"

UCLASS()
class GAMETEMPLATE_API AStageLight : public AActor
{
	GENERATED_BODY()

public:
	AStageLight();

	// Components (assign meshes in BP)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<UStaticMeshComponent> SM_LightBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<UStaticMeshComponent> SM_LightYoke;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<UStaticMeshComponent> SM_LightHead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<USpotLightComponent> SpotLight;

	/** Change the beam color (used by the LightSnapshotManager) */
	UFUNCTION(BlueprintCallable, Category="Light")
	void SetLightColor(const FLinearColor& InColor);

	/** Optional: read current color */
	UFUNCTION(BlueprintPure, Category="Light")
	FLinearColor GetLightColor() const;

protected:
	virtual void BeginPlay() override;
};