// © Anastasis Marinos //

#include "World/Lights/StageLight.h"

AStageLight::AStageLight()
{
	PrimaryActorTick.bCanEverTick = false;

	SM_LightBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LightBase"));
	SetRootComponent(SM_LightBase);
	SM_LightBase->SetMobility(EComponentMobility::Movable);

	SM_LightYoke = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LightYoke"));
	SM_LightYoke->SetupAttachment(SM_LightBase);
	SM_LightYoke->SetMobility(EComponentMobility::Movable);

	SM_LightHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LightHead"));
	SM_LightHead->SetupAttachment(SM_LightYoke);
	SM_LightHead->SetMobility(EComponentMobility::Movable);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(SM_LightHead);
	SpotLight->SetMobility(EComponentMobility::Movable);
}

void AStageLight::BeginPlay()
{
	Super::BeginPlay();
}

/* -------- Native helpers (unchanged behavior) -------- */
void AStageLight::SetBeamColor(const FLinearColor& InColor)
{
	if (SpotLight)
	{
		SpotLight->SetLightColor(InColor, true);
	}
}

FLinearColor AStageLight::ReadBeamColor() const
{
	return SpotLight ? SpotLight->GetLightColor() : FLinearColor::White;
}

/* -------- Interface implementations -------- */
void AStageLight::SetLightColor_Implementation(const FLinearColor& InColor)
{
	SetBeamColor(InColor);
}

FLinearColor AStageLight::GetLightColor_Implementation()
{
	return ReadBeamColor();
}