// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraDirector.generated.h"

class AActor;

UCLASS()
class REAPERSIDESCROLLER_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Camera for first section
	UPROPERTY(EditAnywhere)
		AActor* CameraOne;

	// Camera for second section
	UPROPERTY(EditAnywhere)
		AActor* CameraTwo;

	// Camera for third section
	UPROPERTY(EditAnywhere)
		AActor* CameraThree;

	// Bools for if arrows should be shown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrows")
		bool bFirstArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrows")
		bool bSecondArrow;

	// Box for blocking volume number one
	UPROPERTY(VisibleAnywhere, Category = "Blocking Volume")
		class UBoxComponent* ScreenBlockOne;

	// Box for blocking volume number two
	UPROPERTY(VisibleAnywhere, Category = "Blocking Volume")
		class UBoxComponent* ScreenBlockTwo;

	// Box for backwards blocking volume number one
	UPROPERTY(VisibleAnywhere, Category = "Blocking Volume")
		class UBoxComponent* BackBlockOne;

	// Box for backwards blocking volume number two
	UPROPERTY(VisibleAnywhere, Category = "Blocking Volume")
		class UBoxComponent* BackBlockTwo;

	// Box for move forward zone number one
	UPROPERTY(VisibleAnywhere, Category = "Blocking Volume")
		class UBoxComponent* MoveAheadOne;

	// Box for move forward zone number two
	UPROPERTY(VisibleAnywhere, Category = "Blocking Volume")
		class UBoxComponent* MoveAheadTwo;

	// Declare overlap function for move aheads
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Transitions from one camera to the next
	void CameraGo();

	// Time for transition from one camera to the next
	float TimeToNextCameraChange;

	// Is the camera currently moving?
	bool cameraChange;

	// Which screen blocks have been destroyed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking Volume")
		bool bFirstBlock;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking Volume")
		bool bSecondBlock;

	// Getters for screen block bools so NPC can know which move ahead to activate
	UFUNCTION()
		bool GetBlockingOne();

	UFUNCTION()
		bool GetBlockingTwo();

	// Functions to set collision for move ahead boxes
	UFUNCTION()
		void ActivateOne();

	UFUNCTION()
		void ActivateTwo();

	// Functions to set collision for move ahead boxes
	UFUNCTION()
		void DeactivateOne();

	UFUNCTION()
		void DeactivateTwo();
};
