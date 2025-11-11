// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Interfaces/LightColorTarget.h"
#include "StageLight.generated.h"

UCLASS()
class GAMETEMPLATE_API AStageLight : public AActor, public ILightColorTarget
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

	/** Optional: name of emissive color parameter on SM_LightHead's material[0] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Material")
	FName EmissiveParamName = TEXT("EmissiveColor");

	/** Native C++ convenience (kept) */
	UFUNCTION(BlueprintCallable, Category="Light")
	void SetBeamColor(const FLinearColor& InColor);

protected:
	virtual void BeginPlay() override;

	/** Interface impls */
	virtual void SetLightColor_Implementation(const FLinearColor& InColor) override;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* HeadMID = nullptr;

	FLinearColor LastColor = FLinearColor::White;

	/** Applies emissive to SM_LightHead material[0] if available */
	void ApplyHeadEmissive(const FLinearColor& InColor);
};