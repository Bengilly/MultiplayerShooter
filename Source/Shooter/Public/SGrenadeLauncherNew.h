// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SGrenadeLauncherNew.generated.h"

/**
 *
 */
UCLASS()
class SHOOTER_API ASGrenadeLauncherNew : public ASWeapon
{
	GENERATED_BODY()


protected:
	//  ------------ Variables ------------  //

	UPROPERTY(EditDefaultsOnly, Category = "GrenadeLauncher");
	TSubclassOf<AActor> GrenadeProjectileClass;

	//  ------------ Functions ------------  //

	virtual void ShootWeapon() override;


};
