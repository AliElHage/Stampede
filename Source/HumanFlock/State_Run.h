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
class HUMANFLOCK_API State_Run : public State<ABaseNPC>
{

private:
	// since the state is a singleton, constructor is private
	State_Run() {}

	// copy-ctor and assignment should be private
	State_Run(const State_Run &);
	State_Run& operator= (const State_Run &);

	// turn towards actor if heard and not seen
	void ResetMaxWalkSpeed(ABaseNPC* pNPC);

	// go to dodge state
	void ToDodgeState(ABaseNPC* pNPC);

	// go to flee state
	//void ToFleeState(ABaseNPC* pNPC);

	// the alpha value for the interpolation for the turning around over time of the NPC
	static float DeltaAlpha;

	FVector RandomLocation;

public:
	~State_Run() {}

	static State_Run* State_Run::Instance()
	{
		static State_Run instance;
		return &instance;
	}

	void Enter(ABaseNPC* pNPC) override
	{
		pNPC->SetHasCompletedPath(true);

		pNPC->SetIsRunning(true);

		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("lol"));
		}

		/*FMath::SRandInit(pNPC->GetUniqueID());

		RandomLocation.X = FMath::SRand() * pNPC->ClickDetectedRadius;
		RandomLocation.Y = FMath::SRand() * pNPC->ClickDetectedRadius;
		RandomLocation.Z = 0;*/

		ResetMaxWalkSpeed(pNPC);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Entered: Run State"));
		}
	}

	void Execute(ABaseNPC* pNPC) override
	{

		/*FVector DistanceToDestination = RandomLocation - pNPC->GetActorLocation();
		if (DistanceToDestination.Size() < pNPC->AcceptanceRadius)
		{
			RandomLocation.X = FMath::SRand() * pNPC->ClickDetectedRadius;
			RandomLocation.Y = FMath::SRand() * pNPC->ClickDetectedRadius;
			RandomLocation.Z = 0;
		}*/
		ANPC_Controller* NPC_Controller = Cast<ANPC_Controller>(pNPC->GetController());
		if (NPC_Controller)
		{
			NPC_Controller->MoveToLocation(pNPC->GetRandomLocation(), 0.05f, false);
			if (GEngine && !pNPC->bIsInFlock)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BOLOLOLOLOLOLOLOLOLO"));
			}
		}
		//Cast<ANPC_Controller>(pNPC->GetController())->MoveToLocation(RandomLocation, 0.05f, false);

		FVector TargetLocation = pNPC->GetRandomLocation() - pNPC->GetActorLocation();
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
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::SanitizeFloat(TargetAngle));
		}

		if (pNPC->GetClickedState())
		{
			// Switch to dodge state if the distance between the click and the NPC at the
			// time of the click is within its radius of detection
			FVector Distance = pNPC->GetClickCoordinates() - pNPC->GetInitialPosition();
			if (Distance.Size() < pNPC->ClickDetectedRadius)
			{
				ToDodgeState(pNPC);
			}
		}
	}

	void Exit(ABaseNPC* pNPC) override
	{
		// As soon as the click is detected, we must revert the flag to its original state
		pNPC->SetClickedState(false);

		pNPC->SetIsRunning(false);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Exited: Run State"));
		}
	}

};
