// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseNPC.h"
#include "GameFramework/Character.h"
#include "MyGameInstance.h"
#include "MyPlayerCharacter.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldClickedSignature, FVector, ClickLocation);

UCLASS()
class HUMANFLOCK_API AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	// Click-disabling flag
	bool bHasBeenClicked = false;

	UPROPERTY(EditAnywhere, Category = "GameState")
	bool bIsGameOver = false;

	static int FlockSize;

	UMyGameInstance* GameInstance;

public:
	// Sets default values for this character's properties
	AMyPlayerCharacter();

	// The player controller
	APlayerController* PlayerController;

	// Calculate intersection with desired value
	FVector Intersection(FVector CurrentVector, FVector dir, float Z);

	bool FindActiveCameraInWorld(UCameraComponent* &camComp);

	bool OrthoDeprojectMousePositionToWorld(UCameraComponent* camComp, FVector2D mouseXY, FVector2D viewportSize, FVector& position, FVector& dir);

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnWorldClickedSignature OnPlayerClick;

	UPROPERTY(BlueprintReadWrite, Category = "Visual Effects")
	TSubclassOf<AActor> lightning;

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wGameOverMenu;

	// Variable to hold the widget After Creating it.
	UUserWidget* GameOverMenu;

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wInGameUI;

	// Variable to hold the widget After Creating it.
	UUserWidget* InGameUI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnWorldClick(FVector ClickLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Humans Array")
	TArray<ABaseNPC*> AllHumans;
};
