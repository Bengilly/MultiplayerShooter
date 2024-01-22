#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

UENUM(BlueprintType)
enum class EAbilityPickupType : uint8
{
	Invisibility,
	SpeedBoost
};