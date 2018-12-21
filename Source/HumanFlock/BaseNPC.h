// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "StateMachine.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BaseNPC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveCompletedSignature);
UCLASS()
class HUMANFLOCK_API ABaseNPC : public ACharacter
{
	GENERATED_BODY()

private:
	// An instance of the state machine class
	StateMachine<ABaseNPC>* m_pStateMachine;

	// We must take the click coordinates as a member variable so that the states
	// can access them
	FVector ClickLocation;

	// We must compute the final position given the position of the NPC and the coordinates
	// of the click
	FVector FinalPosition;

	// A random path must be generated to convey a panicked sense to the flock of fleeing NPCs.
	// A new random location is generated every time the current one is reached by the NPC.
	FVector RandomLocation;

	// The initial position of the NPC needs to be kept for the dodge calculations
	FVector InitialPosition;

	// The position from which the NPC starts needs to be tracked for use in the recovery state
	FVector InitialRecoveryPosition;

	// The position to go back to needs to be tracked for use in the recovery state
	FVector RecoveryDestination;

	// The rotation to give the mesh while it is moving
	float MeshRotation = 0.0f;

	// Temporary storage of last rotation for comparison purposes
	float LastMeshRotation = 0.0f;

	// The alpha used to lerp between current rotation and target rotation
	float RotationAlpha = 0.0f;

	// These allow the State Machine to transition from State_Run to State_Dodge
	// without relying on a call from ABaseNPC (a foreign class)
	bool bClick = false;
	bool bHasCompletedPath = true;
	bool bIsRecovering = false;

	// These are used as flags for the Animation BP
	bool bIsRunning = true;
	bool bIsDodging = false;
	bool bIsRegrouping = false;
	bool bIsDying = false;
	bool bIsDead = false;
	bool bHasFallen = false;

public:
	// Sets default values for this character's properties
	ABaseNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// accessor for the member state machine
	StateMachine<ABaseNPC>* GetStateMachine() const { return m_pStateMachine; }

	UFUNCTION()
	void OnClickSensed(FVector ClickLocation);

	UFUNCTION()
	FVector GetClickCoordinates() { return ClickLocation; }

	UFUNCTION()
	FVector GetFinalPosition() { return FinalPosition; }

	UFUNCTION()
	FVector GetInitialPosition() { return InitialPosition; }

	UFUNCTION()
	FVector GetInitialRecoveryPosition() { return InitialRecoveryPosition; }

	UFUNCTION()
	void SetInitialRecoveryPosition(FVector RecoveryPosition) { InitialRecoveryPosition = RecoveryPosition; }

	UFUNCTION()
	FVector GetRecoveryDestination() { return RecoveryDestination; }

	UFUNCTION()
	FVector GetRandomLocation() { return RandomLocation; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	float GetSpeedRatio();

	UFUNCTION()
	void SetRecoveryDestination(FVector RecoveryDestination) { this->RecoveryDestination = RecoveryDestination; }

	UFUNCTION()
	bool GetClickedState() { return bClick; }

	UFUNCTION()
	void SetClickedState(bool isClicked) { bClick = isClicked; }

	UFUNCTION()
	bool GetHasCompletedPath() { return bHasCompletedPath; }

	UFUNCTION()
	void SetHasCompletedPath(bool isCompleted) { bHasCompletedPath = isCompleted; }

	UFUNCTION()
	bool GetIsRecovering() { return bIsRecovering; }

	UFUNCTION()
	void SetIsRecovering(bool isRecovering) { bIsRecovering = isRecovering; }

	UFUNCTION()
	void PathCompleteNotifier();

	UFUNCTION()
	void AssimilateStrayNPC(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	bool GetIsRunning() { return bIsRunning; }

	UFUNCTION()
	void SetIsRunning(bool isRunning) { bIsRunning = isRunning; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	bool GetIsDodging() { return bIsDodging; }

	UFUNCTION()
	void SetIsDodging(bool isDodging) { bIsDodging = isDodging; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	bool GetIsRegrouping() { return bIsRegrouping; }

	UFUNCTION()
	void SetIsRegrouping(bool isRegrouping) { bIsRegrouping = isRegrouping; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	bool GetIsDying() { return bIsDying; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	void SetIsDying(bool isDying) { bIsDying = isDying; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	bool GetIsDead() { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	void SetIsDead(bool isDead) { bIsDead = isDead; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	bool GetHasFallen() { return bHasFallen; }

	UFUNCTION(BlueprintCallable, Category = "FSM States")
	void SetHasFallen(bool hasFallen) { bHasFallen = hasFallen; }

	UFUNCTION(BlueprintCallable, Category = "Mesh Rotation")
	float GetMeshRotation() { return MeshRotation; }

	UFUNCTION(BlueprintCallable, Category = "Mesh Rotation")
	void SetMeshRotation(float myMeshRotation) { MeshRotation = myMeshRotation; }
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Static Mesh")
	UStaticMeshComponent* HumanMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Attributes")
	float NormalDashDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Attributes")
	float MaxDashDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Attributes")
	float AcceptanceRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Attributes")
	float ClickDetectedRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Attributes")
	float RandomLocationRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Key Velocities")
	float InitialDodgeSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Key Velocities")
	float FinalDodgeSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Key Velocities")
	float FinalRecoverySpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Key Velocities")
	float MaxWalkSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flock Behaviour")
	FVector FlockCenter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Option")
	bool bIsInFlock = true;

	UPROPERTY()
	float DeltaTime = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FMoveCompletedSignature OnMoveCompleted;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* AssimilateStrayCapsule;

};
