// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseNPC.h"
#include "State_Dodge.h"
#include "State_Run.h"
#include "State_Stroll.h"
#include "State_Global.h"
#include "MyGameInstance.h"
#include "Engine.h"


// Sets default values
ABaseNPC::ABaseNPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Give the human a state machine
	m_pStateMachine = new StateMachine<ABaseNPC>(this);

	// Set the first and global states
	m_pStateMachine->SetCurrentState(State_Run::Instance());
	m_pStateMachine->SetGlobalState(State_Global::Instance());

	AssimilateStrayCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LedgeGrabCapsule"));
	AssimilateStrayCapsule->InitCapsuleSize(70, 120);
	AssimilateStrayCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AssimilateStrayCapsule->AttachTo(GetRootComponent());

	OnMoveCompleted.AddDynamic(this, &ABaseNPC::PathCompleteNotifier);
	//Super::GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABaseNPC::AssimilateStrayNPC);
	AssimilateStrayCapsule->OnComponentBeginOverlap.AddDynamic(this, &ABaseNPC::AssimilateStrayNPC);

	NormalDashDistance = 350.0f;
	MaxDashDistance = 450.0f;
	AcceptanceRadius = 70.0f;
	ClickDetectedRadius = 400.0f;
	RandomLocationRange = 150.0f;
	InitialDodgeSpeed = 1500.0f;
	FinalDodgeSpeed = 200.0f;
	FinalRecoverySpeed = 700.0f;
	MaxWalkSpeed = 350.0f;
	FlockCenter = FVector(0.0f, 0.0f, 0.0f);

	HumanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HumanMesh"));
	HumanMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
}

// Called when the game starts or when spawned
void ABaseNPC::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		Cast<UMyGameInstance>(GetWorld()->GetGameInstance())->AddNPC(this);
	}

	FMath::SRandInit(GetUniqueID());

	// Get reference to the movement component of the NPC
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

	// If reference was gotten as expected, set the max walk speed to the one passed as a parameter
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = MaxWalkSpeed;
	}

	float SignX, SignY;
	SignX = FMath::SRand();
	SignY = FMath::SRand();
	if (SignX < 0.5f)
		RandomLocation.X = FlockCenter.X + -1 * FMath::SRand() * RandomLocationRange;
	else
		RandomLocation.X = FlockCenter.X + FMath::SRand() * RandomLocationRange;

	if(SignY < 0.5f)
		RandomLocation.Y = FlockCenter.Y + -1 * FMath::SRand() * RandomLocationRange;
	else
		RandomLocation.Y = FlockCenter.Y + FMath::SRand() * RandomLocationRange;

	RandomLocation.Z = 0;
}

// Called every frame
void ABaseNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Make DeltaTime accessible for public use
	this->DeltaTime = DeltaTime;

	// Update the State Machine
	m_pStateMachine->Update();

	if (GetMesh())
	{
		FRotator TargetRotation = FRotator::ZeroRotator;
		TargetRotation.Yaw += MeshRotation;

		FRotator CurrentRotation = GetMesh()->GetComponentRotation();
		CurrentRotation.Pitch = 0.0f;
		CurrentRotation.Roll = 0.0f;

		GetMesh()->SetRelativeRotation(FMath::Lerp(CurrentRotation, TargetRotation, RotationAlpha));
		RotationAlpha += (DeltaTime / 0.5f);
		if (RotationAlpha > 1.0f)
		{
			RotationAlpha = 1.0f;
			if (MeshRotation != LastMeshRotation)
			{
				RotationAlpha = 0.0f;
				if (GEngine)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("LALILUCHE"));
				}
			}
		}

		LastMeshRotation = MeshRotation;
	}
	

	if (bIsDead || GetActorLocation().Y < -2000.0f)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("DESTROY"));
		}
		this->Destroy();
	}
}

// Called to bind functionality to input
void ABaseNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseNPC::OnClickSensed(FVector ClickLocation)
{
	// Notify the run state that a click has been sensed
	bClick = true;

	// Since a dodge is set to happen, this flag must be set to false
	bHasCompletedPath = false;

	// Since clicking gives a new path, the NPC is no longer recovering, no matter
	// its previous state
	bIsRecovering = false;

	// Get the coordinates relevant to the path to be taken
	InitialPosition = GetActorLocation();

	// Compute the raw final position with respect to the click and assign the click
	// coordinates to ClickLocation (used in State_Run to verify if the click is in
	// range).
	FinalPosition = InitialPosition - ClickLocation;
	this->ClickLocation = ClickLocation;

	// Ensure the Z component is 0, as it should always be.
	FinalPosition.Z = 0;

	// This factor will scale the dodge distance according to the initial position of the
	// click with respect to the NPC.
	float Factor = 0.0f;

	// The factor will be greater than LengthMultiplier if the click was closer to the NPC;
	// smaller otherwise.
	Factor = FinalPosition.Size() / NormalDashDistance;
	Factor = NormalDashDistance / Factor;

	// We must gate the Factor so that our NPC doesn't go too far if the click is really close.
	if (Factor > MaxDashDistance) Factor = MaxDashDistance;

	// We compute the scaled FinalPosition, that to which the NPC must dodge towards.
	FinalPosition = FinalPosition.GetSafeNormal() * Factor + InitialPosition;

	// Get reference to the movement component of the NPC
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

	// If reference was gotten as expected, disable the use of acceleration during the initial dodge. This allows
	// the NPC to jump-start its dodge.
	if (MovementComponent)
	{
		MovementComponent->bRequestedMoveUseAcceleration = false;
	}

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FinalPosition.ToString());
	}
}

void ABaseNPC::PathCompleteNotifier()
{
	// Only notify if the dodge or the recovery are complete
	FVector DistanceToDestination = FinalPosition - GetActorLocation();
	if (DistanceToDestination.Size() < AcceptanceRadius && !bIsRecovering)
	{
		// If the dodge has reached its destination, start the recovery step
		bIsRecovering = true;
		//bIsRegrouping = true;

		// Invert InitialPosition and FinalPosition
		FVector Temp = InitialPosition;
		InitialPosition = FinalPosition;
		FinalPosition = Temp;

		// As a safety shield, ClickLocation must always be coupled with FinalPosition
		ClickLocation = Temp;

		// Get reference to the movement component of the NPC
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

		// If reference was gotten as expected, disable the use of acceleration during the initial dodge. This allows
		// the NPC to jump-start its dodge.
		if (MovementComponent)
		{
			MovementComponent->bRequestedMoveUseAcceleration = false;
		}
	}
	else if (DistanceToDestination.Size() < AcceptanceRadius && bIsRecovering)
	{
		// If the NPC recovery has reached its destination, then the recovery flag turns false and the path
		// complete flag turns true.
		bIsRecovering = false;
		bHasCompletedPath = true;
		//bIsRegrouping = false;

		// Get reference to the movement component of the NPC
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

		// If reference was gotten as expected, enable the use of acceleration during the initial dodge. This allows
		// the NPC to return to its position smoothly.
		if (MovementComponent)
		{
			MovementComponent->bRequestedMoveUseAcceleration = true;
		}
	}
	DistanceToDestination = RandomLocation - GetActorLocation();
	if (DistanceToDestination.Size() < AcceptanceRadius)
	{
		float SignX, SignY;
		SignX = FMath::SRand();
		SignY = FMath::SRand();
		if (SignX < 0.5f)
			RandomLocation.X = FlockCenter.X + -1 * FMath::SRand() * RandomLocationRange;
		else
			RandomLocation.X = FlockCenter.X + FMath::SRand() * RandomLocationRange;

		if (SignY < 0.5f)
			RandomLocation.Y = FlockCenter.Y + -1 * FMath::SRand() * RandomLocationRange;
		else
			RandomLocation.Y = FlockCenter.Y + FMath::SRand() * RandomLocationRange;

		RandomLocation.Z = 0;

		// Compute Mesh Rotation for newly generated path
		FVector RL_Temp = RandomLocation;
		RL_Temp.Y += 600.0f;

	}
}

void ABaseNPC::AssimilateStrayNPC(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsInFlock)
	{
		ABaseNPC* pNPC = Cast<ABaseNPC>(OtherActor);
		if (pNPC)
		{
			if (!pNPC->bIsInFlock && !pNPC->GetIsDying())
			{
				pNPC->bIsInFlock = true;
				pNPC->GetStateMachine()->ChangeState(State_Run::Instance());
				if (GetWorld())
				{
					Cast<UMyGameInstance>(GetWorld()->GetGameInstance())->AddNPC(pNPC);
				}
			}
		}
	}
	
}

float ABaseNPC::GetSpeedRatio()
{
	float RelativeSpeed = 0.0f;
	float SpeedRatio = 0.0f;

	RelativeSpeed = RandomLocation.Y - GetActorLocation().Y;

	if (RelativeSpeed > 0.0f)
	{
		SpeedRatio = (600.0f + MaxWalkSpeed) / 600.0f;
	}
	else
	{
		SpeedRatio = (600.0f - MaxWalkSpeed) / 600.0f;
	}

	if (SpeedRatio < 0.0f)
	{
		SpeedRatio = 0.0f;
	}

	return SpeedRatio;
}

