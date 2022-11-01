// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.generated.h"

class AActor;

UCLASS()
class REAPERSIDESCROLLER_API APickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Have we called destroy yet?
	bool bIsDestroying;

	// Collision sphere for pickup action
	UPROPERTY(VisibleAnywhere, Category = "PickUp")
		class UBoxComponent* PickUpBox;

	// Declare the overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Destroy self after pick up complete
	UFUNCTION()
		void DestroyPickUp();
};
