// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NonPlayableDirector.generated.h"

class USceneComponent;

UCLASS()
class REAPERSIDESCROLLER_API ANonPlayableDirector : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* DirectorRoot;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* PointOne;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* PointTwo;
	
public:	
	// Sets default values for this actor's properties
	ANonPlayableDirector();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetNextPoint(bool bIsFirstPoint);
};
