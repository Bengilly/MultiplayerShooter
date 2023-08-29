// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosive.generated.h"

class URadialForceComponent;
class UParticleSyetem;
class UStaticMeshComponent;
class USHealthComponent;


UCLASS()
class SHOOTER_API ASExplosive : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosive();

protected:

	//  ------------ Variables ------------  //

	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "Effects");
	float ExplosionPower;

	UPROPERTY(EditDefaultsOnly, Category = "Effects");
	UParticleSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects");
	UMaterialInterface* ExplodedMaterial;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	//  ------------ Functions ------------  //
	UFUNCTION()
	void OnRep_Exploded();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* ExplosiveHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
