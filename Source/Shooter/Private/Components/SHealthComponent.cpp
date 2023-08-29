// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
USHealthComponent::USHealthComponent()
{
	BaseHealth = 100;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts or when spawned
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//only subscribe to event if client is the server, use GetOwnerRole() as this class is a component
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}

	CurrentHealth = BaseHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, BaseHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Change: %s"), *FString::SanitizeFloat(CurrentHealth));

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}




//  ------------ Multiplayer Functions ------------  //

//run on client
void USHealthComponent::OnRep_Health(float PreviousHealth)
{
	float Damage = CurrentHealth - PreviousHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate to any client connected
	DOREPLIFETIME(USHealthComponent, CurrentHealth);
}