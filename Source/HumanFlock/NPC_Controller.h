// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPC_Controller.generated.h"

/**
 * 
 */
UCLASS()
class HUMANFLOCK_API ANPC_Controller : public AAIController
{
	GENERATED_BODY()
	
public:

	// We must override OnMoveCompleted so that it fires a OnMoveCompleted delegate
	virtual void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) override;
	
	
};
