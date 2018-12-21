// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "BaseNPC.h"
#include "Engine.h"
#include "NPC_Controller.h"

/**
 * 
 */
class HUMANFLOCK_API State_Recover : public State<ABaseNPC>
{
private:
	// since the state is a singleton, constructor is private
	State_Recover() {}

	// copy-ctor and assignment should be private
	State_Recover(const State_Recover &);
	State_Recover& operator= (const State_Recover &);

	// gradually decelerate as approaching target location
	void Accelerate(ABaseNPC* pNPC, float InitialSpeed, float finalSpeed);

	// Set the maximum walk speed of the character
	void SetMaxWalkSpeed(ABaseNPC* pNPC, float MaxWalkSpeed);

	// Go to run state
	void ToRunState(ABaseNPC* pNPC);

	// Go to flee state
	//void ToFleeState(AHuman* pNPC);

	// the alpha value for the interpolation for the turning around over time of the NPC
	static float DeltaAlpha;

public:
	~State_Recover() {}

	// The singleton instance. Returns the existing object or creates a new one 
	static State_Recover* State_Recover::Instance()
	{
		static State_Recover instance;
		return &instance;
	}

	void Enter(ABaseNPC* pNPC) override
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Entered: Recover State"));
		}
	}

	void Execute(ABaseNPC* pNPC) override
	{
		// Get the controller and move to the destination. The boolean must be true if the path is to be interrupted
		// on overlap
		Cast<ANPC_Controller>(pNPC->GetController())->MoveToLocation(pNPC->GetRecoveryDestination(), 0.05f, false);

		// Make the NPC decelerate as he dodges
		//Accelerate(pNPC, pNPC->FinalDodgeSpeed, pNPC->FinalRecoverySpeed);

		// Once the path complete notifier has fired, revert to run state and to original speed, and turn
		// the flag back to false
		if (pNPC->GetHasCompletedPath())
		{
			ToRunState(pNPC);
			SetMaxWalkSpeed(pNPC, 1000.0f);
			pNPC->SetHasCompletedPath(false);
		}
	}

	void Exit(ABaseNPC* pNPC) override
	{
		// For the sake of precision, set the DeltaAlpha back to zero to keep the non-dodging
		// speed consistent
		DeltaAlpha = 0.0f;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Exited: Recover State"));
		}
	}

};
