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
	/** Manager will call this to set the color */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Light")
	void SetLightColor(const FLinearColor& InColor);

	/** Optional readback (manager uses it at startup if needed) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Light")
	FLinearColor GetLightColor();
};