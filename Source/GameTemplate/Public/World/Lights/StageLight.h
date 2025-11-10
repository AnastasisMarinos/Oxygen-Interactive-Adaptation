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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<UStaticMeshComponent> SM_LightBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<UStaticMeshComponent> SM_LightYoke;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<UStaticMeshComponent> SM_LightHead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Components")
	TObjectPtr<USpotLightComponent> SpotLight;

	/** Native C++ convenience (kept) */
	UFUNCTION(BlueprintCallable, Category="Light")
	void SetBeamColor(const FLinearColor& InColor);

	UFUNCTION(BlueprintPure, Category="Light")
	FLinearColor ReadBeamColor() const;

protected:
	virtual void BeginPlay() override;

	/** Interface impls */
	virtual void SetLightColor_Implementation(const FLinearColor& InColor) override;
	virtual FLinearColor GetLightColor_Implementation() override;
};