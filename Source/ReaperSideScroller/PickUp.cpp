// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "ReaperSideScrollerCharacter.h"

// Sets default values
APickUp::APickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create  collision box and set attributes
	PickUpBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	PickUpBox->SetCollisionProfileName(TEXT("OverlapAll"));

	// Add overlap begin and end events to first collision box
	PickUpBox->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnOverlapBegin);

	// Set up hierarchy
	RootComponent = PickUpBox;

	// Start out not being destroyed
	bIsDestroying = false;
}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUp::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		AReaperSideScrollerCharacter* Character = Cast<AReaperSideScrollerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());

		if (OtherActor == Character)
		{
			Character->ResetExposure();
			DestroyPickUp();
		}
	}
}

void APickUp::DestroyPickUp()
{
	if (!bIsDestroying)
	{
		bIsDestroying = true;
		Destroy();
	}
}

