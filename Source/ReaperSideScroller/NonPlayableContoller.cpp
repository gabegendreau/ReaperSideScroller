// Fill out your copyright notice in the Description page of Project Settings.

#include "NonPlayableContoller.h"
#include "NonPlayableCharacter.h"
#include "NonPlayableDirector.h"

ANonPlayableContoller::ANonPlayableContoller()
{
	bIsFirstPoint = true;

	DelaySetting = 3.0f;
}

// Called when the game starts or when spawned
void ANonPlayableContoller::BeginPlay()
{
	Super::BeginPlay();

	MoveToNextPoint();
}

void ANonPlayableContoller::MoveToNextPoint()
{
	// Pointer to NPC that is being controlled
	ThisNonPlayable = Cast<ANonPlayableCharacter>(GetPawn());

	DelaySetting = ThisNonPlayable->GetDelay();

	FVector Destination;

	if (bIsFirstPoint)
	{
		Destination = ThisNonPlayable->GetDirector()->GetNextPoint(bIsFirstPoint);
		bIsFirstPoint = false;
	}
	else
	{
		Destination = ThisNonPlayable->GetDirector()->GetNextPoint(bIsFirstPoint);
		bIsFirstPoint = true;
	}
	MoveToLocation(Destination, 5.0f, true, true);

	FTimerHandle NextPointDelay;
	GetWorldTimerManager().SetTimer(NextPointDelay, this, &ANonPlayableContoller::MoveToNextPoint, DelaySetting, false);
}