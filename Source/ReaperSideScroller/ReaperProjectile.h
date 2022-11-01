// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "ReaperProjectile.generated.h"

class AReaperSideScrollerCharacter;

UCLASS()
class REAPERSIDESCROLLER_API AReaperProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReaperProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The animation to play for projectile
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		class UPaperFlipbookComponent* ProjectileFlipbook;

		class UPaperFlipbook* ProjectileAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AReaperSideScrollerCharacter* ProjCharacter;

	// Collision sphere for registering death's touch
	UPROPERTY(VisibleAnywhere, Category = "Hitting Box")
		class UBoxComponent* HittingBox;

	// Function for killing NPC when hit
	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
