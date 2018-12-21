// Fill out your copyright notice in the Description page of Project Settings.

#include "State_Recover.h"
#include "State_Run.h"

float State_Recover::DeltaAlpha = 0.0f;

void State_Recover::ToRunState(ABaseNPC* pNPC)
{
	// Access the state machine to change the state to the desired one
	pNPC->GetStateMachine()->ChangeState(State_Run::Instance());
}

void State_Recover::Accelerate(ABaseNPC* pNPC, float InitialSpeed, float FinalSpeed)
{
	// Hold reference to the controller
	/*ANPC_Controller* Controller;
	Controller = Cast<ANPC_Controller>(pNPC->GetController());

	// Compute the length of the complete path from initial to final position
	FVector TotalPath = pNPC->GetRecoveryDestination() - pNPC->GetInitialRecoveryPosition();
	float TotalPathLength = TotalPath.Size();

	// Compute the current length from current position to destination. It is a fraction
	// of the total path length.
	FVector CurrentPath = pNPC->GetActorLocation() - pNPC->GetRecoveryDestination();
	float CurrentPathLength = CurrentPath.Size();

	// Lerp from the initial to the final speed of the dodge. The current speed is DeltaAlpha %
	// away from the initial speed: CurrentSpeed = InitialSpeed for DeltaAlpha = 0; = FinalSpeed
	// for DeltaAlpha = 1.
	float CurrentSpeed = FMath::Lerp<float>(InitialSpeed, FinalSpeed, DeltaAlpha);

	// Compute the % of progress of the NPC with respect to his total path
	DeltaAlpha = ((TotalPathLength - CurrentPathLength) / TotalPathLength);

	// Get reference to the movement component of the NPC
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(pNPC->GetMovementComponent());

	// If reference was gotten as expected, set the max walk speed to the computed current speed
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = CurrentSpeed;
	}*/
}

void State_Recover::SetMaxWalkSpeed(ABaseNPC* pNPC, float MaxWalkSpeed)
{
	// Get reference to the movement component of the NPC
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(pNPC->GetMovementComponent());

	// If reference was gotten as expected, set the max walk speed to the one passed as a parameter
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = MaxWalkSpeed;
	}
}
