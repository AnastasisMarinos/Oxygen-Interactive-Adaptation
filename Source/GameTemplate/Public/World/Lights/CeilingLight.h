// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Interfaces/LightColorTarget.h"
#include "CeilingLight.generated.h"

UCLASS()
class GAMETEMPLATE_API ACeilingLight : public AActor, public ILightColorTarget
{
	GENERATED_BODY()

public:
	ACeilingLight();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Panel")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Name of the emissive color parameter on the material */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Panel")
	FName EmissiveParamName = TEXT("EmissiveColor");

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* MID = nullptr;

	FLinearColor LastColor = FLinearColor::White;

	/* ILightColorTarget */
	virtual void SetLightColor_Implementation(const FLinearColor& InColor) override;
};