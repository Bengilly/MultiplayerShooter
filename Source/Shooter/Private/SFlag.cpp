// Fill out your copyright notice in the Description page of Project Settings.


#include "SFlag.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SCharacter.h"

// Sets default values
ASFlag::ASFlag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

	//FlagHolder = nullptr;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    RootComponent = MeshComponent;

    //spin powerup object around the Z axis
    RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
    RotatingComponent->RotationRate.Yaw = 180.0f;
}

// Called when the game starts or when spawned
void ASFlag::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASFlag::NotifyActorBeginOverlap(AActor* Player)
{
    Super::NotifyActorBeginOverlap(Player);

    FlagHolder = Cast<ASCharacter>(Player);

    //broadcast flag pickup to GM and Character class
    OnFlagPickedUp.Broadcast(FlagHolder, this);
}

void ASFlag::OnDropped()
{
    //if (GetLocalRole())
    //{
        //broadcast flag pickup to GM and Character class
        OnFlagDropped.Broadcast(FlagHolder, this);

        FlagHolder = nullptr;
    //}
}

ASCharacter* ASFlag::GetFlagHolder()
{
    return FlagHolder;
}

//replicate the line trace
void ASFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //replicate to any client connected (added condition to avoid replicating on client twice as fire code already executes)
    DOREPLIFETIME_CONDITION(ASFlag, FlagHolder, COND_SkipOwner);
}