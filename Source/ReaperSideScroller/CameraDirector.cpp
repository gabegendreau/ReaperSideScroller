// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"
#include "Kismet/GameplayStatics.h"
#include "ReaperSideScrollerCharacter.h"
#include "GameFramework/Actor.h"
#include "Engine.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create first blocking volume and set collsion profile
	ScreenBlockOne = CreateDefaultSubobject<UBoxComponent>(TEXT("Forward Blocking Volume One"));
	ScreenBlockOne->SetCollisionProfileName(TEXT("BlockAll"));

	// Create second blocking volume and set collsion profile
	ScreenBlockTwo = CreateDefaultSubobject<UBoxComponent>(TEXT("Forward Blocking Volume Two"));
	ScreenBlockTwo->SetCollisionProfileName(TEXT("BlockAll"));

	// Create first backward blocking volume and set collsion profile
	BackBlockOne = CreateDefaultSubobject<UBoxComponent>(TEXT("Back Blocking Volume One"));
	BackBlockOne->SetCollisionProfileName(TEXT("NoCollision"));

	// Create second backward blocking volume and set collsion profile
	BackBlockTwo = CreateDefaultSubobject<UBoxComponent>(TEXT("Back Blocking Volume Two"));
	BackBlockTwo->SetCollisionProfileName(TEXT("NoCollision"));

	// Create first backward blocking volume and set collsion profile
	MoveAheadOne = CreateDefaultSubobject<UBoxComponent>(TEXT("Move Ahead Trigger One"));
	MoveAheadOne->SetCollisionProfileName(TEXT("NoCollision"));

	// Create second backward blocking volume and set collsion profile
	MoveAheadTwo = CreateDefaultSubobject<UBoxComponent>(TEXT("Move Ahead Trigger Two"));
	MoveAheadTwo->SetCollisionProfileName(TEXT("NoCollision"));

	// Overlap events on move ahead triggers
	MoveAheadOne->OnComponentBeginOverlap.AddDynamic(this, &ACameraDirector::OnOverlapBegin);
	MoveAheadTwo->OnComponentBeginOverlap.AddDynamic(this, &ACameraDirector::OnOverlapBegin);

	// Start with blocking volumes marked as in place
	bFirstBlock = true;
	bSecondBlock = true;

	// No arrows to begin with
	bFirstArrow = false;
	bSecondArrow = false;
}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;

	cameraChange = false;
	
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TimeBetweenCameraChange = 2.0f;
	const float SmoothBlendTime = 0.75f;
	TimeToNextCameraChange -= DeltaTime;

	// This logic moves the camera to the next camera based on cameraChange bool
	if (cameraChange)
	{
		// DEV CODE ****************************************
		UE_LOG(LogTemp, Warning, TEXT("cameraChange true!"));

		if (TimeToNextCameraChange <= 0.0f) {
			TimeToNextCameraChange += TimeBetweenCameraChange;

			APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			if (OurPlayerController) {

				// DEV CODE ******************************
				UE_LOG(LogTemp, Warning, TEXT("controller valid"));
				if (CameraTwo)
				{
					UE_LOG(LogTemp, Warning, TEXT("camera two test"));
				}

				if (CameraTwo && (OurPlayerController->GetViewTarget() == CameraOne)) {
					// DEV CODE ******************************
					UE_LOG(LogTemp, Warning, TEXT("camera two"));
					// END ***********************************
					OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
				}
				else if (CameraThree && (OurPlayerController->GetViewTarget() == CameraTwo)) {
					// DEV CODE ******************************
					UE_LOG(LogTemp, Warning, TEXT("camera three"));
					// END ***********************************
					OurPlayerController->SetViewTargetWithBlend(CameraThree, SmoothBlendTime);
				}
				// DEV CODE ******************************
				else if (CameraOne)
				{
					OurPlayerController->SetViewTargetWithBlend(CameraOne, SmoothBlendTime);
					// DEV CODE ******************************
					UE_LOG(LogTemp, Warning, TEXT("camera one"));
					// END ***********************************
				}


			}
		}
		// Allows camera movement to happen again when previous movement is done
		else {
			cameraChange = false;
			TimeToNextCameraChange = 0.0f;
			// DEV CODE ******************************
			UE_LOG(LogTemp, Warning, TEXT("camera ka-ka reset :-P"));
			// END ***********************************
		}
	}
}

// Sets everything proper when new movement begins
void ACameraDirector::CameraGo()
{
	cameraChange = true;
	TimeToNextCameraChange = 0.0f;

	// Remove blocking volumes
	if (bFirstBlock)
	{
		ScreenBlockOne->SetCollisionProfileName(TEXT("NoCollision"));
		BackBlockOne->SetCollisionProfileName(TEXT("BlockAll"));
		bFirstBlock = false;
		ACameraDirector::DeactivateOne();

	}
	else if (bSecondBlock)
	{
		ScreenBlockTwo->SetCollisionProfileName(TEXT("NoCollision"));
		BackBlockTwo->SetCollisionProfileName(TEXT("BlockAll"));
		bSecondBlock = false;
		ACameraDirector::DeactivateTwo();
	}

	// DEV CODE *****************************************
	UE_LOG(LogTemp, Warning, TEXT("cameraGo function activated!"));
}

bool ACameraDirector::GetBlockingOne()
{
	return bFirstBlock;
}

bool ACameraDirector::GetBlockingTwo()
{
	return bSecondBlock;
}

void ACameraDirector::ActivateOne()
{
	MoveAheadOne->SetCollisionProfileName(TEXT("OverlapAll"));
	bFirstArrow = true;
}

void ACameraDirector::ActivateTwo()
{
	MoveAheadTwo->SetCollisionProfileName(TEXT("OverlapAll"));
	bSecondArrow = true;
}

// Overlap begin event for collision box
void ACameraDirector::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// Create a pointer to character
		AReaperSideScrollerCharacter* Character = Cast<AReaperSideScrollerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
		if (OtherActor == Character)
		{
			ACameraDirector::CameraGo();
		}
	}
}

void ACameraDirector::DeactivateOne()
{
	MoveAheadOne->SetCollisionProfileName(TEXT("NoCollision"));
	bFirstArrow = false;
}

void ACameraDirector::DeactivateTwo()
{
	MoveAheadTwo->SetCollisionProfileName(TEXT("NoCollision"));
	bSecondArrow = false;
}