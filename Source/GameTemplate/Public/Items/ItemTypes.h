// © Anastasis Marinos //

#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Headphones	UMETA(DisplayName = "Headphones"),
	Axe			UMETA(DisplayName = "Axe")
};
