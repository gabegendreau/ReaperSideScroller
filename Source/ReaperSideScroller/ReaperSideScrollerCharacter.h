// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Misc/OutputDeviceNull.h"
#include "ReaperSideScrollerCharacter.generated.h"

class ANonPlayableCharacter;

/**
 * This class is the default character for ReaperSideScroller, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class AReaperSideScrollerCharacter : public APaperCharacter
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;
protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	class UPaperFlipbook* IdleAnimation;

	// The animation to play while attacking with scythe
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		class UPaperFlipbook* ScytheAnimation;

	// The animation to play while attacking with death's touch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		class UPaperFlipbook* DeathsTouchAnimation;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for into and out of screen input */
	void MoveAway(float Value);

	// Is character attacking with scythe?
	bool bIsScytheAttack;

	// Is character attacking with death's touch?
	bool bIsDeathsTouchAttack;

	// Is the reaper close enough to melee attack?
	bool bCanAttack;

	// Scythe attack function
	void Scythe();

	// Death's Touch attack function
	void DeathsTouch();

	// Update character
	void UpdateCharacter();

	// Call the cameraGo function from CameraDirector, could not call function from other class using input bindings
	void CallCamera();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	AReaperSideScrollerCharacter();

	 //Collision sphere for being in range of attack
	UPROPERTY(VisibleAnywhere, Category = "Attack Range")
		class USphereComponent* AttackZone;

	// Declare the overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Declare the overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	// Arrow for spawning projectile outside of character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UArrowComponent* ProjArrow;

	// Let me get camera director in UE4
	UPROPERTY(EditAnywhere)
		AActor* MyCameraDirector;

	// Create a pointer to overlapping NPC
	ANonPlayableCharacter* OverlappingNPC;

	// Variable for player's exposure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exposure")
		float Exposure;

	// Variable for tiny amount of exposure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exposure")
		float TinyExposure;

	// Variable for small amount of exposure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exposure")
		float SmallExposure;

	// Variable for large amount of exposure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exposure")
		float LargeExposure;
	
	// To select the blueprint which will contain function to call for red hurt effect
	UPROPERTY(EditAnywhere, Category = "Blueprint Class")
		AActor* BlueprintActor;

	// To select the blueprint which will contain function to call for you won widget
	UPROPERTY(EditAnywhere, Category = "Widget Blueprint")
		AActor* WidgetBlueprint;

	// Increase exposure with amount based on situation
	UFUNCTION(BlueprintCallable, Category = "Exposure")
		void IncExposure(int Amount);

	// Reset exposure to zero
	UFUNCTION(BlueprintCallable, Category = "Exposure")
		void ResetExposure();

	// This will call a function in a blueprint that adds the red hurt screen effect for when exposure is added
	void CallHurtHelper();

	// This will call a function in a blueprint that adds the you lost widget to the screen
	void CallLostWidgetHelper();

	// This will call a function in a blueprint that adds the you won widget to the screen
	void CallWonWidgetHelper();

	// Reset our attack animation bools
	void ResetScythe();

	void ResetDeathsTouch();

	// Getters for attack modes
	bool GetScythe();

	bool GetDeathsTouch();

	// Function for quitting the game
	void ReaperQuit();

	// Function for pausing the game
	void ReaperPause();
};