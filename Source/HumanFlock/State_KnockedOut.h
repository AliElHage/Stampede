// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "BaseNPC.h"
#include "NPC_Controller.h"
#include "Engine.h"

/**
*
*/
class HUMANFLOCK_API State_KnockedOut : public State<ABaseNPC>
{

private:
	// since the state is a singleton, constructor is private
	State_KnockedOut() {}

	// copy-ctor and assignment should be private
	State_KnockedOut(const State_KnockedOut &);
	State_KnockedOut& operator= (const State_KnockedOut &);

	// go to flee state
	//void ToFleeState(ABaseNPC* pNPC);

	// the alpha value for the interpolation for the turning around over time of the NPC
	static float DeltaAlpha;

	float Delay = 0.0f;

public:
	~State_KnockedOut() {}

	static State_KnockedOut* State_KnockedOut::Instance()
	{
		static State_KnockedOut instance;
		return &instance;
	}

	void Enter(ABaseNPC* pNPC) override
	{
		//pNPC->SetIsDying(false);
		Delay = 0.0f;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("DELAY"));
		}
	}

	void Execute(ABaseNPC* pNPC) override
	{
		//pNPC->SetIsDying(true);
		//FVector NewLocation;
		//NewLocation.Y = pNPC->GetActorLocation().Y - 1000.0f * pNPC->DeltaTime;
		//pNPC->SetActorLocation(NewLocation);

		FVector NewLocation = pNPC->GetActorLocation();
		NewLocation.Y = -1000.0f;

		ANPC_Controller* NPC_Controller = Cast<ANPC_Controller>(pNPC->GetController());
		if (NPC_Controller)
		{
			// Get reference to the movement component of the NPC
			UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(pNPC->GetMovementComponent());

			// If reference was gotten as expected, set the max walk speed to the one passed as a parameter
			if (MovementComponent)
			{
				MovementComponent->MaxWalkSpeed = 600.0f;
			}

			NPC_Controller->MoveToLocation(NewLocation, 0.05f, false);
			if (GEngine)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BOLOLOLOLOLOLOLOLOLO"));
			}
		}

		if (Delay > 3.0f)
		{
			pNPC->SetIsDead(true);
		}

		

		Delay += pNPC->DeltaTime;
	}

	void Exit(ABaseNPC* pNPC) override
	{
		
	}

};

