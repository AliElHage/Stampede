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
class HUMANFLOCK_API State_Dodge : public State<ABaseNPC>
{
private:
	// since the state is a singleton, constructor is private
	State_Dodge() {}

	// copy-ctor and assignment should be private
	State_Dodge(const State_Dodge &);
	State_Dodge& operator= (const State_Dodge &);

	// gradually decelerate as approaching target location
	void Decelerate(ABaseNPC* pNPC, float InitialSpeed, float finalSpeed);

	// Set the maximum walk speed of the character
	void SetMaxWalkSpeed(ABaseNPC* pNPC, float MaxWalkSpeed);

	// go to run state
	void ToRunState(ABaseNPC* pNPC);

	// go to recover state
	void ToRecoverState(ABaseNPC* pNPC);

	// the alpha value for the interpolation for the turning around over time of the NPC
	static float DeltaAlpha;

public:
	~State_Dodge() {}

	// The singleton instance. Returns the existing object or creates a new one 
	static State_Dodge* State_Dodge::Instance()
	{
		static State_Dodge instance;
		return &instance;
	}

	void Enter(ABaseNPC* pNPC) override
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Entered: Dodge State"));
		}

		pNPC->SetIsDodging(true);
	}

	void Execute(ABaseNPC* pNPC) override
	{
		// Get the controller and move to the destination. The boolean must be true if the path is to be interrupted
		// on overlap
		Cast<ANPC_Controller>(pNPC->GetController())->MoveToLocation(pNPC->GetFinalPosition(), 0.05f, false);

		// Make the NPC decelerate as he dodges
		if (pNPC->GetIsRecovering())
		{
			Decelerate(pNPC, pNPC->FinalDodgeSpeed, pNPC->FinalRecoverySpeed);
			pNPC->SetIsRegrouping(true);

			/////////////////////////////////////////
			FVector TargetLocation = pNPC->GetFinalPosition() - pNPC->GetActorLocation();
			FVector RunVector = FVector::ZeroVector;
			RunVector.Y += 600.0f;
			TargetLocation.Normalize();
			TargetLocation *= pNPC->MaxWalkSpeed;
			TargetLocation += RunVector;
			float TargetAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RunVector, TargetLocation) / (RunVector.Size() * TargetLocation.Size())));
			if (TargetLocation.X - pNPC->GetActorLocation().X > 0.0f)
			{
				TargetAngle *= -1;
			}
			pNPC->SetMeshRotation(TargetAngle);
			/////////////////////////////////////////
		}
		else
		{
			pNPC->SetIsRegrouping(false);
			Decelerate(pNPC, pNPC->InitialDodgeSpeed, pNPC->FinalDodgeSpeed);

			/////////////////////////////////////////
			FVector TargetLocation = pNPC->GetFinalPosition() - pNPC->GetActorLocation();
			FVector RunVector = FVector::ZeroVector;
			RunVector.Y += 600.0f;
			TargetLocation.Normalize();
			TargetLocation *= pNPC->MaxWalkSpeed;
			TargetLocation += RunVector;
			float TargetAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RunVector, TargetLocation) / (RunVector.Size() * TargetLocation.Size())));
			if (TargetLocation.X - pNPC->GetActorLocation().X > 0.0f)
			{
				TargetAngle *= -1;
			}
			pNPC->SetMeshRotation(TargetAngle);
			/////////////////////////////////////////
		}

		// Once the path complete notifier has fired, revert to run state and to original speed, and turn
		// the flag back to false
		if (pNPC->GetHasCompletedPath())
		{
			ToRunState(pNPC);
		}
	}

	void Exit(ABaseNPC* pNPC) override
	{
		// For the sake of precision, set the DeltaAlpha back to zero to keep the non-dodging
		// speed consistent
		DeltaAlpha = 0.0f;

		pNPC->SetIsDodging(false);

		pNPC->SetHasCompletedPath(false);

		pNPC->SetInitialRecoveryPosition(pNPC->GetActorLocation());

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Exited: Dodge State"));
		}
	}

};
