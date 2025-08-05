#include "SWolf.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/AudioComponent.h"

ASWolf::ASWolf()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	//GetMesh()->SetWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
	//RootComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	//set default values
	WolfDamage = 50.0f;
	WalkSpeed = 200.0f;
	RunSpeed = 350.0f;

	bIsChasing = false;
	bHasGrowled = false;

	bIsWalking = false;
	bIsRunning = false;
	bIsBiting = false;
	bIsDead = false;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASWolf::BeginPlay()
{
	Super::BeginPlay();



	
	//get the player character reference
	PlayerCharacter = Cast<ASCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called every frame
void ASWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		//handle patrol or chasing the player
		if (bIsChasing)
		{
			ChasePlayer();
		}
		else
		{
			Patrol(DeltaTime);
		}
	}

}

//called when the AI has not sensed a player to chase
void ASWolf::Patrol(float DeltaTime)
{
	bIsRunning = false;
	bIsWalking = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

//called the AI has sensed a player to chase
void ASWolf::ChasePlayer()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Wolf is chasing player")));

	//update animation and increase speed
	bIsWalking = false;
	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

}

//attack the player - task called from behaviour tree
void ASWolf::AttackPlayer(ASCharacter* PlayerToAttack)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerAttackPlayer(PlayerToAttack);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Green, FString::Printf(TEXT("Wolf attacking player")));

	bIsBiting = true;

	//apply damage to player
	UGameplayStatics::ApplyDamage(PlayerToAttack, WolfDamage, GetController(), this, nullptr);
}

void ASWolf::ServerAttackPlayer_Implementation(ASCharacter* PlayerToAttack)
{
	AttackPlayer(PlayerToAttack);
}

//called from the healthcomponent to handle death of AI actor	
void ASWolf::OnDeath()
{
	//update animation
	bIsDead = true;

	//stop behaviour tree logic
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AIController->BrainComponent);
		if (BTComp)
		{
			BTComp->StopTree(EBTStopMode::Safe); //stops current BT logic
		}

		//stop wolf movement
		AIController->StopMovement(); 
	}

	//stop any audio currently being played from the wolf
	if (WolfAudioComponent && WolfAudioComponent->IsPlaying())
	{
		//UE_LOG(LogTemp, Log, TEXT("Stopping wolf sound"));
		WolfAudioComponent->Stop();
	}

	SetLifeSpan(10.0f);
}

void ASWolf::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWolf, bIsWalking);
	DOREPLIFETIME(ASWolf, bIsRunning);
	DOREPLIFETIME(ASWolf, bIsBiting);
	DOREPLIFETIME(ASWolf, bIsDead);
	DOREPLIFETIME(ASWolf, bIsChasing);
	DOREPLIFETIME(ASWolf, bHasGrowled);
}