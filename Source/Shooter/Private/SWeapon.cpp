// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Shooter/Shooter.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

//add console variable to enable/disable debug lines
static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("Shooter.ShowDebugLines"),DebugWeaponDrawing,TEXT("Show debug lines for weapons"),ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "TracerEnd";

	BodyDamage = 15.0f;
	HeadshotDamage = 50.0f;
	RateOfFire = 500;

	SetReplicates(true);
	//reduce impact of latency on weapon object
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::ShootWeapon()
{

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerShootWeapon();
	}

	//trace world from pawn head to crosshair
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector Eyelocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(Eyelocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = Eyelocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		//particle "Target" parameter for smoke trail
		FVector WeaponTracerEndPoint = TraceEnd;
		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Eyelocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			//apply damage depending on surface hit with trace
			float WeaponDamage;

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			if (SurfaceType == SURFACE_HEADSHOT)
			{
				WeaponDamage = HeadshotDamage;
			}
			else
			{
				WeaponDamage = BodyDamage;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			//tracer collided - update end impact point
			WeaponTracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			//show debug lines of shot
			DrawDebugLine(GetWorld(), Eyelocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayWeaponEffects(WeaponTracerEndPoint);

		//update struct with line trace info to update clients
		if (GetLocalRole() == ROLE_Authority)
		{
			LineTrace.TraceEnd = WeaponTracerEndPoint;
			LineTrace.SurfaceType = SurfaceType;
		}

		TimeSinceLastShot = GetWorld()->TimeSeconds;
	}

}

void ASWeapon::StartShooting()
{
	//pick between highest value (essentially clamping value to 0.0f if first is <0.0f)
	float ShootDelay = FMath::Max(TimeSinceLastShot + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	//if shootdelay is <0 it will use TimeBetweenShots delay value instead
	GetWorldTimerManager().SetTimer(TimerHandler_TimeBetweenShots,this,&ASWeapon::ShootWeapon, TimeBetweenShots, true, ShootDelay);
}

void ASWeapon::StopShooting()
{
	GetWorldTimerManager().ClearTimer(TimerHandler_TimeBetweenShots);
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector TraceImpactLocation)
{
	//select which impact effect to play depending on surface hit by weapon trace
	UParticleSystem* ImpactEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_BODYSHOT:
		ImpactEffect = ImpactEffectBodyshot;
		break;
	case SURFACE_HEADSHOT:
		ImpactEffect = ImpactEffectHeadshot;
		break;
	default:
		ImpactEffect = ImpactEffectDefault;
		break;
	}

	//spawn impact effect if assigned
	if (ImpactEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector TraceDirection = TraceImpactLocation - MuzzleLocation;
		TraceDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, TraceImpactLocation, TraceDirection.Rotation());
	}
}


void ASWeapon::PlayWeaponEffects(FVector TracerEndPoint)
{
	//spawn muzzle flash if effect is assigned
	if (MuzzleFlashEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEffect, MeshComp, MuzzleSocketName);
	}

	if (WeaponTracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* WeaponTracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponTracerEffect, MuzzleLocation);

		if (WeaponTracerComp)
		{
			WeaponTracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}

	//get playercontroller and play camera shake
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(CameraShake);
		}
	}
}

//  ------------ Multiplayer Functions ------------  //

//server function - shoot weapon
void ASWeapon::ServerShootWeapon_Implementation()
{
	ShootWeapon();
}

//validate code for anti-cheat
bool ASWeapon::ServerShootWeapon_Validate()
{
	return true;
}

//add weapon fx on all clients
void ASWeapon::OnRep_LineTrace()
{
	PlayWeaponEffects(LineTrace.TraceEnd);
	PlayImpactEffects(LineTrace.SurfaceType, LineTrace.TraceEnd);
}

//replicate the line trace
void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate to any client connected (added condition to avoid replicating on client twice as fire code already executes)
	DOREPLIFETIME_CONDITION(ASWeapon, LineTrace, COND_SkipOwner);
}