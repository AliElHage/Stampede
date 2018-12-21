// Fill out your copyright notice in the Description page of Project Settings.

#include "State_Run.h"
#include "State_Dodge.h"

void State_Run::ToDodgeState(ABaseNPC* pNPC)
{
	// Access the state machine to change the state to the desired one
	pNPC->GetStateMachine()->ChangeState(State_Dodge::Instance());
}

void State_Run::ResetMaxWalkSpeed(ABaseNPC* pNPC)
{
	// Get reference to the movement component of the NPC
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(pNPC->GetMovementComponent());

	// If reference was gotten as expected, set the max walk speed to the one passed as a parameter
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = pNPC->MaxWalkSpeed;
	}
}
