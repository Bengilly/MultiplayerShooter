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
#include "TimerManager.h"
#include "Logging/StructuredLog.h"
#include "SPowerupObject.h"
#include "SPowerupBase.h"

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

	MaxStamina = 100.0f;
	CurrentStamina = 100.0f;
	StaminaRegenRate = 5.0f;
	StaminaUsageRateSprinting = 10.0f;
	StaminaUsageRateJumping = 20.0f;

	AbilityIndex = 0;
	//SelectedAbility = nullptr;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComponent->FieldOfView;

	HealthComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	//initialise player weapons on the server
	if (GetLocalRole() == ROLE_Authority)
	{
		InitialiseWeapon(RifleWeaponClass, PlayerRifleAmmo, RifleAttachSocketName);
		InitialiseWeapon(PistolWeaponClass, PlayerPistolAmmo, PistolAttachSocketName);
		if (WeaponStructArray.Num() > 0)
		{
			//equip rifle by default
			EquipWeapon(WeaponStructArray[0].Weapon);
		}

		InitialiseAbility(InvisibilityClass, 5);
		InitialiseAbility(SpeedBoostClass, 5);
		if (AbilityStructArray.Num() > 0)
		{
			EquipAbility(AbilityStructArray[0].Ability);
		}
	}

}

//spawn weapons on player
void ASCharacter::InitialiseWeapon(TSubclassOf<ASWeapon> WeaponClass, int Ammo, FName SocketName)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASWeapon* Weapon = GetWorld()->SpawnActor<ASWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	//WeaponAmmoMap.Add(Weapon, Ammo);

	WeaponInfoStruct.Weapon = Weapon;
	WeaponInfoStruct.Ammo = Ammo;

	WeaponStructArray.Add(WeaponInfoStruct);

	AddWeapon(WeaponInfoStruct.Weapon);
}

void ASCharacter::AddWeapon(ASWeapon* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->SetOwner(this);
		WeaponClassArray.AddUnique(Weapon);
	}
}

/*	future refactor to avoid duplicate weapon switching code	*/
void ASCharacter::SwitchToRifle()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (!CanSwitchWeapon(WeaponClassArray[0])) { return; }

		UE_LOG(LogTemp, Log, TEXT("Rifle Ammo: %s"), *FString::SanitizeFloat(WeaponStructArray[0].Ammo));

		bIsSwitchingWeapon = true;		//replicate weaponswitch animation/audio
		PlayAnimMontage(SwitchWeaponAnim);
		CurrentWeapon->PlayUnEquipAudio();	//play sound specific to weapon already equipped

		//start timer to switch weapon
		FTimerDelegate TimerDel_SwitchToRifle;
		TimerDel_SwitchToRifle.BindUFunction(this, "EquipWeapon", WeaponClassArray[0]);
		GetWorld()->GetTimerManager().SetTimer(TimerHandler_SwitchWeapon, TimerDel_SwitchToRifle, 1.00, false);
	}
	else
	{
		ServerSwitchToRifle();
	}
}

/*	future refactor to avoid duplicate weapon switching code	*/
void ASCharacter::SwitchToPistol()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (!CanSwitchWeapon(WeaponClassArray[1])) { return; }

		UE_LOG(LogTemp, Log, TEXT("Pistol Ammo: %s"), *FString::SanitizeFloat(WeaponStructArray[1].Ammo));

		bIsSwitchingWeapon = true;		//replicate weaponswitch animation/audio
		PlayAnimMontage(SwitchWeaponAnim);
		CurrentWeapon->PlayUnEquipAudio();		//play sound specific to weapon already equipped

		//start timer to switch weapon
		FTimerDelegate TimerDel_SwitchToPistol;
		TimerDel_SwitchToPistol.BindUFunction(this, "EquipWeapon", WeaponClassArray[1]);
		GetWorld()->GetTimerManager().SetTimer(TimerHandler_SwitchWeapon, TimerDel_SwitchToPistol, 1.00, false);
	}
	else
	{
		ServerSwitchToPistol();
	}
}

void ASCharacter::EquipWeapon(ASWeapon* Weapon)
{
	///* add function to start timer before switching meshes to sync up with client animation */
	//ensure the weapon is set by the server
	if (Weapon)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);		//pass in the new weapon to equip and previously equiped weapon
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}

	GetWorldTimerManager().ClearTimer(TimerHandler_SwitchWeapon);
}

void ASCharacter::OnRep_StartWeaponSwitch()
{
	if (bIsSwitchingWeapon)
	{
		PlayAnimMontage(SwitchWeaponAnim);
		CurrentWeapon->PlayUnEquipAudio();	//play sound specific to weapon already equipped
	}
}

void ASCharacter::OnRep_ChangeCurrentWeapon(ASWeapon* PreviousWeapon)
{
	SetCurrentWeapon(CurrentWeapon, PreviousWeapon);
}

void ASCharacter::SetCurrentWeapon(ASWeapon* NewWeapon, ASWeapon* PreviousWeapon)
{
	ASWeapon* LastWeapon = nullptr;

	if (PreviousWeapon != NULL)
	{
		LastWeapon = PreviousWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LastWeapon = CurrentWeapon;
	}

	if (LastWeapon)
	{
		LastWeapon->GetWeaponMesh()->SetHiddenInGame(true);
	}

	//trigger OnRep_WeaponSwitch to replicate weapon switching to clients
	CurrentWeapon = NewWeapon;
	bIsSwitchingWeapon = false;

	NewWeapon->PlayEquipAudio();	//plays sound specific to weapon being equipped
	NewWeapon->SetOwner(this);
	NewWeapon->GetWeaponMesh()->SetHiddenInGame(false);
}

bool ASCharacter::CanSwitchWeapon(ASWeapon* Weapon)
{
	if (!CurrentWeapon || CurrentWeapon==Weapon || bIsShooting || bIsReloading || bIsZooming || bIsSwitchingWeapon)
	{
		UE_LOG(LogTemp, Log, TEXT("Weapon Switch Blocked"));
		return false;
	}
	else
	{
		return true;
	}
}

void ASCharacter::StartReload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (bIsShooting || bIsReloading || PlayerRifleAmmo == 0 || PlayerPistolAmmo == 0 || CurrentWeapon->CheckIfMagazineFull())
	{
		UE_LOG(LogTemp, Log, TEXT("Reload blocked"));
		return;
	}
	else 
	{
		bIsReloading = true;
		PlayAnimMontage(ReloadAnim);
		UGameplayStatics::PlaySoundAtLocation(this, StartReloadSound, this->GetActorLocation());

		//Bind delegate to use function parameters
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, "ReloadWeapon", CurrentWeapon);
		GetWorld()->GetTimerManager().SetTimer(Timerhandle_Reload, TimerDel, 2.17, false);
	}
}

void ASCharacter::OnRep_Reloading()
{
	if (bIsReloading)
	{
		PlayAnimMontage(ReloadAnim);
		UGameplayStatics::PlaySoundAtLocation(this, StartReloadSound, this->GetActorLocation());
	}
}

//reload ammo depending on current weapon equipped
void ASCharacter::ReloadWeapon(ASWeapon* EquippedWeapon)
{
	int AmmoToReload = EquippedWeapon->QueryAmmoMissing();
	
	//int* Ammo = WeaponAmmoMap.Find(EquippedWeapon);

	int Ammo;

	for (int i = 0; i < WeaponStructArray.Num(); i++)
	{
		if (EquippedWeapon == WeaponStructArray[i].Weapon)
		{
			Ammo = WeaponStructArray[i].Ammo;

			if (Ammo >= AmmoToReload)
			{
				Ammo -= AmmoToReload;
				EquippedWeapon->Reload(AmmoToReload);

				GetWorldTimerManager().ClearTimer(Timerhandle_Reload);
				UGameplayStatics::PlaySoundAtLocation(this, EndReloadSound, this->GetActorLocation());
				UE_LOG(LogTemp, Log, TEXT("Remaining Player Ammo: %s"), *FString::SanitizeFloat(Ammo));
			}
			else if (Ammo < AmmoToReload)
			{
				//if player doesn't have enough ammo to refill magazine, add remaining bullets to current mag
				EquippedWeapon->Reload(Ammo);
				Ammo = 0;
			}
			WeaponStructArray[i].Ammo = Ammo;
		}
	}
	bIsReloading = false;
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

//  ------------ Ability Functions ------------  //

void ASCharacter::AddPowerupChargeToPlayer(TSubclassOf<ASPowerupObject> Powerup, int Charges)
{
	if (!Powerup)
	{
		UE_LOG(LogTemp, Log, TEXT("Powerup object is null"));
		return;
	}
	
	//if (AbilityArray.Contains(Powerup))
	//{
		for (int i = 0; i < AbilityStructArray.Num(); i++)
		{
			if (AbilityStructArray[i].AbilityType == Powerup)
			{
				AbilityStructArray[i].NumberOfCharges += Charges;
		
				FString ClassName = Powerup->GetName();
				UE_LOG(LogTemp, Log, TEXT("Ability: %s"), *ClassName);
				UE_LOG(LogTemp, Log, TEXT("Total charges: %d"), AbilityStructArray[i].NumberOfCharges);
			}
		}
	//}
}

void ASCharacter::InitialiseAbility(TSubclassOf<ASPowerupBase> AbilityClass, int Charges)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	ASPowerupBase* Ability = GetWorld()->SpawnActor<ASPowerupBase>(AbilityClass);

	if (Ability)
	{
		AbilityInfoStruct.Ability = Ability;
		AbilityInfoStruct.AbilityType = AbilityClass;
		AbilityInfoStruct.NumberOfCharges = Charges;

		AbilityInfoStruct.Ability->SetOwner(this);
		AbilityStructArray.Add(AbilityInfoStruct);

		UE_LOG(LogTemp, Log, TEXT("Ability Initialised: %s"), *FString(AbilityInfoStruct.Ability->GetName()));
		UE_LOG(LogTemp, Log, TEXT("Ability Charges: %d"), AbilityInfoStruct.NumberOfCharges);		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability Class is null"));
	}
}

//void ASCharacter::AddAbility(ASPowerupBase* Ability)
//{
//	if (Ability && GetLocalRole() == ROLE_Authority)
//	{
//		Ability->SetOwner(this);
//		AbilityClassArray.AddUnique(Ability);
//	}
//}

void ASCharacter::UseAbility()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			UE_LOG(LogTemp, Log, TEXT("Trying to use ability..."));

			//check to see what ability is being used and subtract charge 
			if (SelectedAbility)
			{
				for (int i = 0; i < AbilityStructArray.Num(); i++)
				{
					if (SelectedAbility == AbilityStructArray[i].Ability && AbilityStructArray[i].NumberOfCharges > 0)
					{
						UE_LOG(LogTemp, Log, TEXT("Ability Used: %s"), *FString(SelectedAbility->GetName()));

						AbilityStructArray[i].Ability->ActivateAbility(this);
						AbilityStructArray[i].NumberOfCharges -= 1;
					}
				}
			}
		}
	}
	else
	{
		ServerUseAbility();
	}
}

void ASCharacter::SwitchNextAbility()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Switching Ability"))

		if (AbilityStructArray.Num() > AbilityIndex + 1)
		{
			++AbilityIndex;

			if (ASPowerupBase* NextAbility = AbilityStructArray[AbilityIndex].Ability) //need this?
			{
				UE_LOG(LogTemp, Log, TEXT("Attempting to select ability..."));

				EquipAbility(AbilityStructArray[AbilityIndex].Ability);
				//SelectedAbility = AbilityStructArray[AbilityIndex].Ability;		//need to replicate to clients??

				UE_LOG(LogTemp, Log, TEXT("Ability selected: %s"), *FString(SelectedAbility->GetName()));

			}
		}
	}
	else
	{
		ServerSwitchNextAbility();
	}
}

void ASCharacter::SwitchPreviousAbility()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Switching Ability"))

		if (AbilityIndex - 1 >= 0)
		{
			--AbilityIndex;

			if (ASPowerupBase* NextAbility = AbilityStructArray[AbilityIndex].Ability)
			{
				UE_LOG(LogTemp, Log, TEXT("Attempting to select ability..."));

				EquipAbility(AbilityStructArray[AbilityIndex].Ability);
				//SelectedAbility = AbilityStructArray[AbilityIndex].Ability;

				UE_LOG(LogTemp, Log, TEXT("Ability selected: %s"), *FString(SelectedAbility->GetName()));

			}
		}
	}
	else
	{
		ServerSwitchPreviousAbility();
	}
}

void ASCharacter::OnRep_ChangeAbility()
{
	SetCurrentAbility(SelectedAbility);
}

void ASCharacter::SetCurrentAbility(ASPowerupBase* NewSelectedAbility)
{
	SelectedAbility = NewSelectedAbility;
	//SelectedAbility->SetOwner(this);
}

void ASCharacter::EquipAbility(ASPowerupBase* NewSelectedAbility)
{
	///* add function to start timer before switching meshes to sync up with client animation */

	//ensure the weapon is set by the server
	if (NewSelectedAbility)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			SetCurrentAbility(NewSelectedAbility);		//pass in the new weapon to equip and previously equiped weapon
		}
		else
		{
			ServerEquipAbility(NewSelectedAbility);
		}
	}
}

void ASCharacter::ServerUseAbility_Implementation()
{
	UseAbility();
}
void ASCharacter::ServerSwitchNextAbility_Implementation()
{
	SwitchNextAbility();
}
void ASCharacter::ServerSwitchPreviousAbility_Implementation()
{
	SwitchPreviousAbility();
}
void ASCharacter::ServerEquipAbility_Implementation(ASPowerupBase* NewSelectedAbility)
{
	EquipAbility(NewSelectedAbility);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//true = zoomed fov, false = defaultfov
	float TargetFOV = bIsZooming ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComponent->SetFieldOfView(NewFOV);

	UpdateStamina(DeltaTime);
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
	if (bIsZooming)
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
	if (CurrentWeapon && !bIsReloading && !bSprinting && !bIsSwitchingWeapon)
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

//handles stamina usage/regen // future work to separate into timers with more control
void ASCharacter::UpdateStamina(float DeltaTime)
{
	if (bSprinting && CurrentStamina > 0)
	{
		//depleting
		CurrentStamina = FMath::FInterpConstantTo(CurrentStamina, 0.0f, DeltaTime, StaminaUsageRateSprinting);
	}
	else
	{
		//regenerating
		CurrentStamina = FMath::FInterpConstantTo(CurrentStamina, MaxStamina, DeltaTime, StaminaRegenRate);
	}

	if (CurrentStamina == 0)
	{
		StopSprinting();
	}
}

void ASCharacter::StartJumping()
{
	//player can only jump if not currently zooming or jumping
	if (!bIsZooming && !GetCharacterMovement()->IsFalling())
	{
		if (CurrentStamina > StaminaUsageRateJumping)
		{
			//reduce current stamina when jumping
			CurrentStamina -= StaminaUsageRateJumping;
			ACharacter::Jump();
		}
	}
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::StartJumping);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::ZoomIn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::ZoomOut);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartShooting);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopShooting);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::StartReload);
	
	PlayerInputComponent->BindAction("EquipRifle", IE_Pressed, this, &ASCharacter::SwitchToRifle);
	PlayerInputComponent->BindAction("EquipPistol", IE_Pressed, this, &ASCharacter::SwitchToPistol);

	PlayerInputComponent->BindAction("UseAbility", IE_Pressed, this, &ASCharacter::UseAbility);
	PlayerInputComponent->BindAction("SwitchNextAbility", IE_Pressed, this, &ASCharacter::SwitchNextAbility);
	PlayerInputComponent->BindAction("SwitchPreviousAbility", IE_Pressed, this, &ASCharacter::SwitchPreviousAbility);

	//PlayerInputComponent->BindAction<FUseAbilityDelegate>("UseAbility", IE_Pressed, this, &ASCharacter::UseAbility, Ability);
	
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

void ASCharacter::ServerStartReload_Implementation()
{
	StartReload();
}

void ASCharacter::ServerReloadWeapon_Implementation(ASWeapon* EquippedWeapon)
{
	ReloadWeapon(EquippedWeapon);
}

void ASCharacter::ServerEquipWeapon_Implementation(ASWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

void ASCharacter::ServerSwitchToRifle_Implementation()
{
	SwitchToRifle();
}

void ASCharacter::ServerSwitchToPistol_Implementation()
{
	SwitchToPistol();
}

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

	DOREPLIFETIME(ASCharacter, SelectedAbility);
	DOREPLIFETIME(ASCharacter, AbilityInfoStruct);
	DOREPLIFETIME(ASCharacter, AbilityStructArray);	

	//DOREPLIFETIME(ASCharacter, AbilityIndex);
	//DOREPLIFETIME(ASCharacter, AbilityClassArray);
	//DOREPLIFETIME_CONDITION(ASCharacter, AbilityClassArray, COND_OwnerOnly);

	DOREPLIFETIME(ASCharacter, bPlayerDied);
	DOREPLIFETIME(ASCharacter, bIsZooming);
	DOREPLIFETIME(ASCharacter, bSprinting);
	DOREPLIFETIME(ASCharacter, bIsReloading);
	DOREPLIFETIME(ASCharacter, bIsShooting);
	DOREPLIFETIME(ASCharacter, bIsSwitchingWeapon);

	DOREPLIFETIME(ASCharacter, PlayerRifleAmmo);
	DOREPLIFETIME(ASCharacter, PlayerPistolAmmo);

	DOREPLIFETIME(ASCharacter, WeaponInfoStruct);
	DOREPLIFETIME(ASCharacter, WeaponStructArray);
	
	DOREPLIFETIME(ASCharacter, SwitchWeaponAnim);

	DOREPLIFETIME_CONDITION(ASCharacter, WeaponClassArray, COND_OwnerOnly);
}