// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReaperSideScrollerGameMode.h"
#include "ReaperSideScrollerCharacter.h"

AReaperSideScrollerGameMode::AReaperSideScrollerGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = AReaperSideScrollerCharacter::StaticClass();	
}
