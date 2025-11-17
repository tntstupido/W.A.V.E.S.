// Source/Waves/Private/Conductor_Waves.cpp

#include "Conductor_Waves.h"

#include "Kismet/GameplayStatics.h"

AConductor_Waves::AConductor_Waves()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AConductor_Waves::BeginPlay()
{
	Super::BeginPlay();

	// Play music
	if (MusicTrack)
	{
		UGameplayStatics::PlaySound2D(this, MusicTrack);
	}

	// Set up beat timer
	const float BeatLength = (BPM > 0.f) ? 60.f / BPM : 0.5f;

	BeatCounter = 0;
	BarCounter = 0;

	if (BeatLength > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_Beat,
			this,
			&AConductor_Waves::HandleBeat,
			BeatLength,
			true  // looping
		);
	}
}

void AConductor_Waves::HandleBeat()
{
	// Broadcast simple beat index (0,1,2,...)
	OnBeat.Broadcast(BeatCounter);

	// Compute beat within bar
	const int32 BeatInBar = (BeatsPerBar > 0) ? (BeatCounter % BeatsPerBar) : BeatCounter;

	// Advance bar counter when a new bar starts
	if (BeatInBar == 0 && BeatCounter > 0)
	{
		++BarCounter;
	}

	OnBar.Broadcast(BarCounter, BeatInBar);

	++BeatCounter;
}
