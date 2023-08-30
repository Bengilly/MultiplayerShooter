// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupBase.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
ASPowerupBase::ASPowerupBase()
{
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(64, 60, 60);
	DecalComponent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(60.0f);
	RootComponent = SphereComponent;
}

// Called when the game starts or when spawned
void ASPowerupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	//todo: actions when powerup collected

}