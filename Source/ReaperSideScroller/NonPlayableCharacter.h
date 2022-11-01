// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbook.h"
#include "NonPlayableCharacter.generated.h"

class USphereComponent;
class ANonPlayableDirector;

UCLASS()
class REAPERSIDESCROLLER_API ANonPlayableCharacter : public APaperCharacter
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;
protected:

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		class UPaperFlipbook* WalkingAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		class UPaperFlipbook* IdleAnimation;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	// Update character
	void UpdateCharacter();

public:
	ANonPlayableCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Type")
		bool bIsMarked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Type")
		bool bIsStationary;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsExposed;

	// Collision sphere for registering death's touch
	UPROPERTY(VisibleAnywhere, Category = "Death's Touch Sphere")
		class USphereComponent* DeathsTouchSphere;

	// Collision sphere for nearby NPCs to calculate exposure with scythe death
	UPROPERTY(VisibleAnywhere, Category = "NPC Proximity Sphere")
		class USphereComponent* ProximitySphere;

	// Is NPC dying
	bool bIsDying;

	// Int to keep track of NPCs nearby when killed
	int NearbyChars;

	void Killed();

	void ResetExposed();

	void DeathDeath();

	void CallQuit();

	int GetNearby();

	// Let me get camera director in UE4
	UPROPERTY(EditAnywhere)
		AActor* MyCameraDirector;

	// Let me pick a movement director from the scene so NPC can move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		ANonPlayableDirector* MovementDirector;

	// Return the diretor attached to this NPC
	UFUNCTION()
		ANonPlayableDirector* GetDirector();

	// Time for delay until next move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delay")
		float DelayTime;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetDelay();

	// Function for player bumping into NPC
	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Declare the overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Declare the overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);
};
