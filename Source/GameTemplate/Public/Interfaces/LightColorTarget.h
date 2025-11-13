// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LightColorTarget.generated.h"

UINTERFACE(BlueprintType)
class ULightColorTarget : public UInterface
{
	GENERATED_BODY()
};
class ILightColorTarget
{
	GENERATED_BODY()

public:
	// Function called to set the color on light actors
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Light")
	void SetLightColor(const FLinearColor& InColor);

	// Function returns the color that's set on light actors
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Light")
	FLinearColor GetLightColor();
};