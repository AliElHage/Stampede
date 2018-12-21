// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC_Controller.h"
#include "BaseNPC.h"
#include "Engine.h"

void ANPC_Controller::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	ABaseNPC* ControllerOwner = Cast<ABaseNPC>(this->GetPawn());
	if (ControllerOwner)
	{
		ControllerOwner->OnMoveCompleted.Broadcast();
	}
}


