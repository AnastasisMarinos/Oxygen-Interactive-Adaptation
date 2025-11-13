// © Anastasis Marinos //

#include "World/Lights/StageLight.h"
#include "Materials/MaterialInstanceDynamic.h"

AStageLight::AStageLight()
{
	PrimaryActorTick.bCanEverTick = false;

	SM_LightBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LightBase"));
	SetRootComponent(SM_LightBase);

	SM_LightYoke = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LightYoke"));
	SM_LightYoke->SetupAttachment(SM_LightBase);

	SM_LightHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LightHead"));
	SM_LightHead->SetupAttachment(SM_LightYoke);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(SM_LightHead);
}

void AStageLight::BeginPlay()
{
	Super::BeginPlay();

	// Create a MID for the material on the head
	if (SM_LightHead && SM_LightHead->GetMaterial(0))
	{
		HeadMID = SM_LightHead->CreateDynamicMaterialInstance(0);
	}

	// Initialize material to match the current light
	if (SpotLight)
	{
		LastColor = SpotLight->GetLightColor();
		ApplyHeadEmissive(LastColor);
	}
}

void AStageLight::SetBeamColor(const FLinearColor& InColor)
{
	LastColor = InColor;

	if (SpotLight)
	{
		SpotLight->SetLightColor(InColor, true);
	}

	ApplyHeadEmissive(InColor);
}

void AStageLight::SetLightColor_Implementation(const FLinearColor& InColor)
{
	SetBeamColor(InColor);
}

// Applies emissive to SM_LightHead material[0] if available
void AStageLight::ApplyHeadEmissive(const FLinearColor& InColor)
{
	if (HeadMID && EmissiveParamName != NAME_None)
	{
		HeadMID->SetVectorParameterValue(EmissiveParamName, InColor);
	}
}