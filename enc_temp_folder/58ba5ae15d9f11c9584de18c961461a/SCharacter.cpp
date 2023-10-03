// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Shooter/Shooter.h"
#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComonent"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComp);

	ZoomedFOV = 60.0f;
	ZoomInterpSpeed = 10.0f;

	RifleAttachSocketName = "Socket_Rifle";
	PistolAttachSocketName = "Socket_Pistol";

	PlayerRifleAmmo = 90;
	PlayerPistolAmmo = 30;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComponent->FieldOfView;

	HealthComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	//only run on server
	if (GetLocalRole() == ROLE_Authority)
	{
		//spawn default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(RifleWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RifleAttachSocketName);
		}
	}
}

void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ASCharacter::MoveRight(float value)
{
	if (bSprinting)
	{
		return;
	}

	AddMovementInput(GetActorRightVector() * value);
}

void ASCharacter::BeginCrouch()
{
	if(bIsZooming)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = 100.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	}

	Crouch();
}

void ASCharacter::EndCrouch()
{
	if (bIsZooming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	}

	UnCrouch();
}

void ASCharacter::ZoomIn()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerZoomIn();
	}

	if (bSprinting)
	{
		bSprinting = false;
	}

	bIsZooming = true;
	

	if (GetCharacterMovement()->bWantsToCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = 100.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
}

void ASCharacter::ZoomOut()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerZoomOut();
	}

	bIsZooming = false;

	if (GetCharacterMovement()->bWantsToCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
	}
}

void ASCharacter::StartShooting()
{
	if (CurrentWeapon && !bIsReloading && !bSprinting)
	{
		bIsShooting = true;
		CurrentWeapon->StartShooting();
	}
}

void ASCharacter::StopShooting()
{
	if (CurrentWeapon && bIsShooting)
	{
		CurrentWeapon->StopShooting();
		bIsShooting = false;
	}
}

void ASCharacter::StartSprinting()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartSprinting();
	}

	if (bIsZooming)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	bSprinting = true;
}

void ASCharacter::StopSprinting()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopSprinting();
	}

	if (bIsZooming)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	bSprinting = false;
}

void ASCharacter::StartReload()
{
	if (bIsShooting || bIsReloading || PlayerRifleAmmo == 0 || CurrentWeapon->GetCurrentAmmo() == 30)
	{
		UE_LOG(LogTemp, Log, TEXT("Reload blocked"));
		return;
	}
	else 
	{
		bIsReloading = true;
		PlayAnimMontage(ReloadMontage);
		UGameplayStatics::PlaySoundAtLocation(this, StartReloadSound, this->GetActorLocation());

		GetWorldTimerManager().SetTimer(Timerhandle_Reload, this, &ASCharacter::ReloadWeapon, 2.17, false);
	}
}

void ASCharacter::ReloadWeapon()
{
	int AmmoToReload = CurrentWeapon->QueryAmmoMissing();

	if (PlayerRifleAmmo >= AmmoToReload)
	{
		PlayerRifleAmmo -= AmmoToReload;
		CurrentWeapon->Reload(AmmoToReload);

		GetWorldTimerManager().ClearTimer(Timerhandle_Reload);

		UGameplayStatics::PlaySoundAtLocation(this, EndReloadSound, this->GetActorLocation());

		UE_LOG(LogTemp, Log, TEXT("Remaining Player Ammo: %s"), *FString::SanitizeFloat(PlayerRifleAmmo));
	}
	else if(PlayerRifleAmmo < AmmoToReload)
	{
		CurrentWeapon->Reload(PlayerRifleAmmo);
		PlayerRifleAmmo = 0;
	}

	bIsReloading = false;
}

void ASCharacter::EquipRifle()
{
	//only run on server
	if (GetLocalRole() == ROLE_Authority)
	{
		//spawn default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(RifleWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RifleAttachSocketName);
		}
	}
}

void ASCharacter::EquipPistol()
{
	//only run on server
	if (GetLocalRole() == ROLE_Authority)
	{
		//spawn default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(PistolWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, PistolAttachSocketName);
		}
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* CharacterHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bPlayerDied)
	{
		//player dies
		bPlayerDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//true = zoomed fov, false = defaultfov
	float TargetFOV = bIsZooming ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComponent->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::ZoomIn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::ZoomOut);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartShooting);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopShooting);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::StartReload);
	
	PlayerInputComponent->BindAction("EquipRifle", IE_Pressed, this, &ASCharacter::EquipRifle);
	PlayerInputComponent->BindAction("EquipPistol", IE_Pressed, this, &ASCharacter::EquipPistol);
}

//setup line trace from camera
FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}


//  ------------ Multiplayer Functions ------------  //

void ASCharacter::ServerZoomIn_Implementation()
{
	ZoomIn();
}

void ASCharacter::ServerZoomOut_Implementation()
{
	ZoomOut();
}

void ASCharacter::ServerStartSprinting_Implementation()
{
	StartSprinting();
}

void ASCharacter::ServerStopSprinting_Implementation()
{
	StopSprinting();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate variables
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bPlayerDied);
	DOREPLIFETIME(ASCharacter, bIsZooming);
	DOREPLIFETIME(ASCharacter, bSprinting);
}