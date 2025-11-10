// © Anastasis Marinos //
#include "World/Lights/CeilingLight.h"

ACeilingLight::ACeilingLight()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
}

void ACeilingLight::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh && Mesh->GetMaterial(0))
	{
		MID = Mesh->CreateDynamicMaterialInstance(0);
	}
}

void ACeilingLight::SetLightColor_Implementation(const FLinearColor& InColor)
{
	LastColor = InColor;
	if (MID && EmissiveParamName != NAME_None)
	{
		// Push in linear space; shader usually expects sRGB->linear already
		MID->SetVectorParameterValue(EmissiveParamName, InColor * EmissiveBoost);
	}
}

FLinearColor ACeilingLight::GetLightColor_Implementation()
{
	return LastColor;
}