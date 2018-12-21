// Fill out your copyright notice in the Description page of Project Settings.

#include "State_Dodge.h"
#include "State_Run.h"
#include "State_Recover.h"

float State_Dodge::DeltaAlpha = 0.0f;

void State_Dodge::ToRunState(ABaseNPC* pNPC)
{
	// Access the state machine to change the state to the desired one
	pNPC->GetStateMachine()->ChangeState(State_Run::Instance());
}

void State_Dodge::ToRecoverState(ABaseNPC* pNPC)
{
	// Access the state machine to change the state to the desired one
	pNPC->GetStateMachine()->ChangeState(State_Recover::Instance());
}

void State_Dodge::Decelerate(ABaseNPC* pNPC, float InitialSpeed, float FinalSpeed)
{
	// Hold reference to the controller
	ANPC_Controller* Controller;
	Controller = Cast<ANPC_Controller>(pNPC->GetController());

	// Compute the length of the complete path from initial position to click
	FVector TotalPath = pNPC->GetFinalPosition() - pNPC->GetInitialPosition();
	float TotalPathLength = TotalPath.Size();

	// Compute the current length from current position to click. It is a fraction
	// of the total path length.
	FVector CurrentPath = pNPC->GetFinalPosition() - pNPC->GetActorLocation();
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
	}
}

void State_Dodge::SetMaxWalkSpeed(ABaseNPC* pNPC, float MaxWalkSpeed)
{
	// Get reference to the movement component of the NPC
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(pNPC->GetMovementComponent());

	// If reference was gotten as expected, set the max walk speed to the one passed as a parameter
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = MaxWalkSpeed;
	}
}