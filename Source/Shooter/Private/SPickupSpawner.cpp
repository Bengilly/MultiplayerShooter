#include "SPickupSpawner.h"
#include <Kismet/KismetMathLibrary.h>
#include "SPickupObject.h"
#include "SPowerupObject.h"
#include "Shooter/Shooter.h"

// Sets default values
ASPickupSpawner::ASPickupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASPickupSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (Landscape)
	{


		// Call the spawn function
		SpawnPickupObjects();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Landscape reference is null"));
	}
}

void ASPickupSpawner::SpawnPickupObjects()
{
	if (!PickupObjectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActorToSpawn reference is null"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World reference is null"));
		return;
	}

	// Get the bounds of the landscape
	FBox LandscapeBounds = Landscape->GetComponentsBoundingBox();

	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Spawning pickups...")));

	//loop through and spawn pickup objects in random locations within the landscape bounds
	for (int32 i = 0; i < NumberOfActorsToSpawn; i++)
	{

		// Generate a random X and Y within the landscape bounds
		float RandomX = FMath::RandRange(0.0f, 50400.0f);
		float RandomY = FMath::RandRange(0.0f, 50400.0f);
		//GEngine->AddOnScreenDebugMessage(-1, 30.0, FColor::Green, FString::Printf(TEXT("X: %s | Y: %s"), *FString::SanitizeFloat(RandomX), *FString::SanitizeFloat(RandomY)));
		
		//set the location values for the trace
		FVector Start = FVector(RandomX, RandomY, LandscapeBounds.Max.Z + 3200.0f);
		FVector End = FVector(RandomX, RandomY, LandscapeBounds.Min.Z - 3200.0f);
		//GEngine->AddOnScreenDebugMessage(-1, 30.0, FColor::Green, FString::Printf(TEXT("Start: %s | End: %s"),*Start.ToString(),*End.ToString()));

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, COLLISION_OBJECTSPAWNER, QueryParams);

		//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Calculate line trace...")));

		//if the line trice hits the surface, spawn actor
		if (bHit)
		{
			DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Green, true, -1.0f, 0, 5.0f);
			//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Hit landscape...")));
			UE_LOG(LogTemp, Warning, TEXT("Linetrace hit"));

			FVector SpawnLocation = HitResult.ImpactPoint;
			SpawnLocation.Z += 20.0f;

			//setup spawn parameters & spawn pickup object
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			PickupObjectInstance = World->SpawnActor<ASPickupObject>(PickupObjectClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("No linetrace hit")));
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, -1.0f, 0.0f, 2.0f);
			UE_LOG(LogTemp, Warning, TEXT("No linetrace hit"));
		}
	}
}

// Called every frame
void ASPickupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}