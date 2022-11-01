// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "ReaperSideScrollerCharacter.h"
#include "NonPlayableContoller.h"
#include "Engine.h"
#include "ReaperProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "CameraDirector.h"

//////////////////////////////////////////////////////////////////////////
// ANonPlayableCharacter

ANonPlayableCharacter::ANonPlayableCharacter()
{
	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Ad OnHit event to capsule component
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ANonPlayableCharacter::OnCompHit);
	
	// Prevent all automatic rotation behavior on the character
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	// ************ Modified to allow 2.5 depth walk movement ************
	GetCharacterMovement()->bConstrainToPlane = false;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Create  collision sphere and set attributes
	DeathsTouchSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Death's Touch Collision"));
	DeathsTouchSphere->SetCollisionProfileName(TEXT("OverlapAll"));
	DeathsTouchSphere->SetupAttachment(RootComponent);
	DeathsTouchSphere->SetSphereRadius(100.0f);
	DeathsTouchSphere->SetHiddenInGame(true);

	// Create collision sphere and set attributes
	ProximitySphere = CreateDefaultSubobject<USphereComponent>(TEXT("Proximity Sphere"));
	ProximitySphere->SetCollisionProfileName(TEXT("OverlapAll"));
	ProximitySphere->SetupAttachment(RootComponent);
	ProximitySphere->SetSphereRadius(200.0f);
	ProximitySphere->SetHiddenInGame(true);

	// Add overlap begin and end events to ProximitySphere collision sphere
	ProximitySphere->OnComponentBeginOverlap.AddDynamic(this, &ANonPlayableCharacter::OnOverlapBegin);
	ProximitySphere->OnComponentEndOverlap.AddDynamic(this, &ANonPlayableCharacter::OnOverlapEnd);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	// Set being exposed to false for on hit event
	bIsExposed = false;

	// Can't start dead
	bIsDying = false;

	// Default exp for nearby npcs when killed
	NearbyChars = 0;

	DelayTime = 3.0f;

	MovementDirector = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ANonPlayableCharacter::UpdateAnimation()
{
	const FVector NPCVelocity = GetVelocity();
	const float NPCSpeedSqr = NPCVelocity.SizeSquared();

	// Are we doing an attack, moving, or standing still?
	UPaperFlipbook* DesiredAnimation = (NPCSpeedSqr > 0.0f) ? WalkingAnimation : IdleAnimation;
	if (NPCSpeedSqr > 0.0f)
	{
		DesiredAnimation = WalkingAnimation;
	}
	else
	{
		DesiredAnimation = IdleAnimation;
	}
	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ANonPlayableCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCharacter();
}

void ANonPlayableCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector NPCVelocity = GetVelocity();
	float TravelDirection = NPCVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void ANonPlayableCharacter::Killed()
{
	// bool to prevent calling this on an object pending destroy
	bIsDying = true;

	// Create a pointer to character
	AReaperSideScrollerCharacter* Character = Cast<AReaperSideScrollerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());

	if (!bIsMarked)
	{
		Character->IncExposure(3);
		if (Character->GetScythe())
		{
			// Let scythe animation finish before destroying
			FTimerHandle ScytheDelay;
			GetWorldTimerManager().SetTimer(ScytheDelay, this, &ANonPlayableCharacter::DeathDeath, .53f, false);
		}
		else
		{
		Destroy();
		}
	}
	else
	{
		ACameraDirector* CameraDirectorPointer = Cast<ACameraDirector>(MyCameraDirector);

		if (CameraDirectorPointer->GetBlockingOne())
		{
			CameraDirectorPointer->ActivateOne();
		}
		else if (CameraDirectorPointer->GetBlockingTwo())
		{
			CameraDirectorPointer->ActivateTwo();
		}
		else
		{
			// calls function from player character to get won widget to the screen
			Character->CallWonWidgetHelper();

			// Delay and call quit game function
			FTimerHandle WonTimerHandle;
			GetWorldTimerManager().SetTimer(WonTimerHandle, this, &ANonPlayableCharacter::CallQuit, .05f, false);
		}

		if (Character->GetScythe())
		{
			// Let scythe animation finish before destroying
			FTimerHandle ScytheDelay;
			GetWorldTimerManager().SetTimer(ScytheDelay, this, &ANonPlayableCharacter::DeathDeath, 0.53f, false);
		}
		else
		{
			Destroy();
		}
	}
}

ANonPlayableDirector* ANonPlayableCharacter::GetDirector()
{
	//if (MovementDirector != nullptr)
	//{
		return MovementDirector;
	//}
}

// OnCompHit for sending tiny exp to character when bumps into
void ANonPlayableCharacter::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// Create a pointer to character
	AReaperSideScrollerCharacter* Character = Cast<AReaperSideScrollerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (OtherActor == Character)
		{
			if (!bIsExposed)
			{
				bIsExposed = true;
				// Reaper is exposed
				Character->IncExposure(1);
				FTimerHandle ExpHandle;
				GetWorldTimerManager().SetTimer(ExpHandle, this, &ANonPlayableCharacter::ResetExposed, 1.0f, false);
			}
		}
	}
}

void ANonPlayableCharacter::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (Cast<ANonPlayableCharacter>(OtherActor))
		{
			NearbyChars += 1;
		}
	}
}

void ANonPlayableCharacter::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (Cast<ANonPlayableCharacter>(OtherActor))
		{
			NearbyChars -= 1;
		}
	}
}

void ANonPlayableCharacter::ResetExposed()
{
	ANonPlayableCharacter::bIsExposed = false;
}

void ANonPlayableCharacter::DeathDeath()
{
	Destroy();
}

int ANonPlayableCharacter::GetNearby()
{
	return NearbyChars;
}

float ANonPlayableCharacter::GetDelay()
{
	return DelayTime;
}

void ANonPlayableCharacter::CallQuit()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("NPC call quit (actually calls pause)"));
	}

	// Create a pointer to character
	AReaperSideScrollerCharacter* Character = Cast<AReaperSideScrollerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());

	if (Character)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Character pointer in NPC is valid"));
		}
	}

	Character->ReaperPause();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("After pause called in NPC"));
	}
}