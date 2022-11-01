// Fill out your copyright notice in the Description page of Project Settings.


#include "ReaperProjectile.h"
#include "NonPlayableCharacter.h"
#include "ReaperSideScrollerCharacter.h"
#include "PaperFlipbook.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AReaperProjectile::AReaperProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjCharacter = Cast<AReaperSideScrollerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// Create flipbook component
	ProjectileFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Projectile Flipbook"));

	ProjectileAnim = LoadObject<UPaperFlipbook>(nullptr, TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/Aseprite/Projectile/projectile.projectile'"));

	ProjectileFlipbook->SetFlipbook(ProjectileAnim);
	
	// Create  collision sphere and set attributes
	HittingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Projectile Collision"));
	HittingBox->SetCollisionProfileName(TEXT("BlockAll"));
	HittingBox->SetBoxExtent(FVector(12.0f, 36.0f, 12.0f));
	HittingBox->SetHiddenInGame(true);
	HittingBox->SetNotifyRigidBodyCollision(true);
	HittingBox->OnComponentHit.AddDynamic(this, &AReaperProjectile::OnCompHit);

	// Set up hierarchy
	RootComponent = HittingBox;
	ProjectileFlipbook->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AReaperProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AReaperProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// X axis value is speed
	AReaperProjectile::AddActorLocalOffset(FVector(3.5f, 0.0f, 0.0f), true);

}

// On Hit for death's touch to NPC
void AReaperProjectile::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// Create a pointer to hit NPC
	ANonPlayableCharacter* HitCharacter = Cast<ANonPlayableCharacter>(OtherActor);

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (OtherActor == HitCharacter)
		{
			HitCharacter->Killed();
		}
	}

	// This was at the top of the function but I moved it down here to avoid problems
	Destroy();
}