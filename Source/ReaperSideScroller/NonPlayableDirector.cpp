// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableDirector.h"
#include "Components/SceneComponent.h"

// Sets default values
ANonPlayableDirector::ANonPlayableDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DirectorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = DirectorRoot;

	PointOne = CreateDefaultSubobject<USceneComponent>(TEXT("MovePointOne"));
	PointOne->SetupAttachment(RootComponent);

	PointTwo = CreateDefaultSubobject<USceneComponent>(TEXT("MovePointTwo"));
	PointTwo->SetupAttachment(RootComponent);
}

FVector ANonPlayableDirector::GetNextPoint(bool bIsFirstPoint)
{
	if (bIsFirstPoint)
	{
		return PointOne->GetComponentLocation();
	}
	else
	{
		return PointTwo->GetComponentLocation();
	}
}