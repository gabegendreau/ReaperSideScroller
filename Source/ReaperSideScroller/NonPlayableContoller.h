// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "NonPlayableContoller.generated.h"

class ANonPlayableCharacter;

UCLASS()
class REAPERSIDESCROLLER_API ANonPlayableContoller : public AAIController
{
	GENERATED_BODY()

public:

	ANonPlayableContoller();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function for getting next point to move to
	UFUNCTION()
		void MoveToNextPoint();
	
	// Pointer to NPC that is being controlled
	ANonPlayableCharacter* ThisNonPlayable;

	// bool for flip flip
	bool bIsFirstPoint;

	float DelaySetting;
};
