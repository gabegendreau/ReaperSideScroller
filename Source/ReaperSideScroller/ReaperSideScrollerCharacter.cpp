// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReaperSideScrollerCharacter.h"
//#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "ReaperProjectile.h"
#include "GameFramework/Actor.h"
#include "NonPlayableCharacter.h"
#include "Engine.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"
#include "CameraDirector.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// AReaperSideScrollerCharacter

AReaperSideScrollerCharacter::AReaperSideScrollerCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create  collision sphere and set attributes
	AttackZone = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Zone Sphere"));
	AttackZone->SetCollisionProfileName(TEXT("OverlapAll"));

	// Add overlap begin and end events to first collision box
	AttackZone->OnComponentBeginOverlap.AddDynamic(this, &AReaperSideScrollerCharacter::OnOverlapBegin);
	AttackZone->OnComponentEndOverlap.AddDynamic(this, &AReaperSideScrollerCharacter::OnOverlapEnd);

	// Arrow component for spawning projectile away from character
	ProjArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Projectile Arrow"));

	// Prevent all automatic rotation behavior on the character
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Starting exposure amount and amounts for different exposure situations
	Exposure = 0.0f;
	TinyExposure = 5.0f;
	SmallExposure = 10.0f;
	LargeExposure = 15.0f;

	// Start out with no attack bools true
	bIsDeathsTouchAttack = false;
	bIsScytheAttack = false;
	bCanAttack = false;

	// Pointer to OverlappingNPC must start as null or else... OR ELSE...
	OverlappingNPC = nullptr;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	// ************ Modified to allow 2.5 depth walk movement ************
	GetCharacterMovement()->bConstrainToPlane = false;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	// Set input mode - is there a function to set focus to viewport/window? - This probably shouldn't be in the player character but putting it in the game mode crashed the editor - jk it crashes here too
	//APlayerController* ReaperController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//ReaperController->SetInputMode(FInputModeGameOnly());
}

void AReaperSideScrollerCharacter::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Cast<ANonPlayableCharacter>(OtherActor))
	{
		bCanAttack = true;
		OverlappingNPC = Cast<ANonPlayableCharacter>(OtherActor);
	}
}

void AReaperSideScrollerCharacter::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ANonPlayableCharacter>(OtherActor))
	{
		bCanAttack = false;
		OverlappingNPC = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
// Animation

void AReaperSideScrollerCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we doing an attack, moving, or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if (bIsScytheAttack)
	{
		DesiredAnimation = ScytheAnimation;
	}
	else if (bIsDeathsTouchAttack)
	{
		DesiredAnimation = DeathsTouchAnimation;
	}
	else if (PlayerSpeedSqr > 0.0f)
	{
		DesiredAnimation = RunningAnimation;
	}
	else
	{
		DesiredAnimation = IdleAnimation;
	}
	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AReaperSideScrollerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacter();	
}


//////////////////////////////////////////////////////////////////////////
// Input

void AReaperSideScrollerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	// Other note: 'Jump' has been commented out and the input keys in project settings have been removed ...
	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	// PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Scythe", IE_Pressed, this, &AReaperSideScrollerCharacter::Scythe);
	PlayerInputComponent->BindAction("DeathsTouch", IE_Pressed, this, &AReaperSideScrollerCharacter::DeathsTouch);
	// DEV CODE ************************************
	PlayerInputComponent->BindAction("CallCamera", IE_Pressed, this, &AReaperSideScrollerCharacter::CallCamera);
	// END *****************************************
	PlayerInputComponent->BindAxis("MoveRight", this, &AReaperSideScrollerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveAway", this, &AReaperSideScrollerCharacter::MoveAway);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AReaperSideScrollerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AReaperSideScrollerCharacter::TouchStopped);
}

// Performs the scythe attack
void AReaperSideScrollerCharacter::Scythe()
{
	bIsScytheAttack = true;
	if (bCanAttack)
	{
		if (OverlappingNPC != nullptr)
		{
			int NumNearby = OverlappingNPC->GetNearby();
			if (NumNearby > 0)			// This will need to be changed when this code scales up! IT WILL BREAK IF MORE THAN 3 NPCs ARE NEARBY, which is currently impossible and in the future improbable, but just fix it
			{
				IncExposure(NumNearby);
			}
			OverlappingNPC->Killed();
		}
	}
	FTimerHandle ScytheTimerHandle;
	GetWorldTimerManager().SetTimer(ScytheTimerHandle, this, &AReaperSideScrollerCharacter::ResetScythe, 0.66f, false);
}

// Performs the Death's Touch attack if exposure is zero
void AReaperSideScrollerCharacter::DeathsTouch()
{
	if (Exposure == 0)
	{
		if (!bIsDeathsTouchAttack) {
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("DEATH'S TOUCH ATTACK!!"));
		}*/
		bIsDeathsTouchAttack = true;
		// Don't loop animation
		FTimerHandle DeathsTouchTimerHandle;
		GetWorldTimerManager().SetTimer(DeathsTouchTimerHandle, this, &AReaperSideScrollerCharacter::ResetDeathsTouch, 0.35f, false);

		FVector loc(ProjArrow->GetComponentLocation());
		FRotator rot(ProjArrow->GetComponentRotation());
		FActorSpawnParameters SpawnInfo;
		GetWorld()->SpawnActor<AReaperProjectile>(loc, rot, SpawnInfo);
		}
	}
	else
	{
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Exposure must be 0 to use Death's Touch"));
		}*/
	}
}

// Increase exposure by specified amount
void AReaperSideScrollerCharacter::IncExposure(int Amount)
{
	switch (Amount)
	{
	case 1:
		Exposure += TinyExposure;
		break;
	case 2:
		Exposure += SmallExposure;
		break;
	case 3:
		Exposure += LargeExposure;
		break;
	default: 
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("A proper int representing exposure amount was not passed."));
		}
	}

	//flash
	CallHurtHelper();

	if (Exposure >= 100)
	{
		// Call function to get lost widget ot the screen
		CallLostWidgetHelper();

		// Delay and call quit game function
		FTimerHandle LostTimerHandle;
		GetWorldTimerManager().SetTimer(LostTimerHandle, this, &AReaperSideScrollerCharacter::ReaperPause, .05f, false);
	}
}

// Reset exposure to zero
void AReaperSideScrollerCharacter::ResetExposure()
{
	Exposure = 0.0f;
}

void AReaperSideScrollerCharacter::CallHurtHelper()
{
	FOutputDeviceNull ar;
	const FString command = FString::Printf(TEXT("MakeHurtEffect"));
	if (BlueprintActor)
	{
		BlueprintActor->CallFunctionByNameWithArguments(*command, ar, NULL, true);
	}
}

void AReaperSideScrollerCharacter::CallLostWidgetHelper()
{
	FOutputDeviceNull ar;
	const FString lostcommand = FString::Printf(TEXT("MakeLostWidget"));
	if (WidgetBlueprint)
	{
		WidgetBlueprint->CallFunctionByNameWithArguments(*lostcommand, ar, NULL, true);
	}
}

void AReaperSideScrollerCharacter::CallWonWidgetHelper()
{
	FOutputDeviceNull ar;
	const FString woncommand = FString::Printf(TEXT("MakeWonWidget"));
	if (WidgetBlueprint)
	{
		WidgetBlueprint->CallFunctionByNameWithArguments(*woncommand, ar, NULL, true);
	}
}

// Moves the character left and right on the screen
void AReaperSideScrollerCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(0.3f, 0.0f, 0.0f), Value);
}

// Moves the character away and towards the player, into and out of the screen
void AReaperSideScrollerCharacter::MoveAway(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(0.0f, -1.0f, 0.0f), Value);
}

void AReaperSideScrollerCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	//Jump();
}

void AReaperSideScrollerCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	//StopJumping();
}

void AReaperSideScrollerCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void AReaperSideScrollerCharacter::CallCamera()
{
	ACameraDirector* CameraDirectorPointer = Cast<ACameraDirector>(MyCameraDirector);
	CameraDirectorPointer->CameraGo();
}

void AReaperSideScrollerCharacter::ResetScythe()
{
	AReaperSideScrollerCharacter::bIsScytheAttack = false;
}

void AReaperSideScrollerCharacter::ResetDeathsTouch()
{
	AReaperSideScrollerCharacter::bIsDeathsTouchAttack = false;
}

bool AReaperSideScrollerCharacter::GetScythe()
{
	return bIsScytheAttack;
}

// MIGHT NOT NEED THIS, I DON'T KNOW WHAT I WAS THINKING
bool AReaperSideScrollerCharacter::GetDeathsTouch()
{
	return bIsDeathsTouchAttack;
}

// Quit game
void AReaperSideScrollerCharacter::ReaperQuit()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Reaper quit function started"));
	}

	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

// Pause game and call quit
void AReaperSideScrollerCharacter::ReaperPause()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Reaper pause function start"));
	}

	// Cut off player input to reaper character
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	this->DisableInput(PlayerController);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Reaper input disabled"));
	}

	//Removing because I cannot get it to work
	//// Get and destroy all NPCs
	//TArray<AActor*> AllNonPlayables;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANonPlayableCharacter::StaticClass(), AllNonPlayables);
	//int ArrayLength = sizeof(AllNonPlayables)/sizeof(AllNonPlayables[0]);

	//FString IntAsString = FString::FromInt(ArrayLength);
	//long long SizeOfVector = sizeof(AllNonPlayables);
	//FString LongAsString = FString::FromInt(SizeOfVector);

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, IntAsString);
	//}

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LongAsString);
	//}

	//for (int i = 0; i < ArrayLength; i++)
	//{
	//	AllNonPlayables[i]->SetActorHiddenInGame(true);	// ********** THIS IS ABOUT WHERE YOU WANT TO BE WORKING ************
	//	if (GEngine)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("This was one pass through our for loop"));
	//	}
	//}

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("NPCs destroyed... ...well, hidden"));
	//}

	// Delay and call quit game function
	FTimerHandle PauseQuitTimerHandle;
	GetWorldTimerManager().SetTimer(PauseQuitTimerHandle, this, &AReaperSideScrollerCharacter::ReaperQuit, 4.0f, false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("This executes within ReaperPause but after the timer call to ReaperQuit"));
	}
}