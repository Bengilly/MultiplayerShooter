// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeLauncherNew.h"

void ASGrenadeLauncherNew::ShootWeapon()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner && GrenadeProjectileClass)
	{
		FVector Eyelocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(Eyelocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(GrenadeProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
	}
}