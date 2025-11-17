#include "Spawner_Wave.h"
#include "Actor_Wave.h"
#include "GS_Waves.h"
#include "DA_DifficultyProfile.h"
#include "Engine/World.h"
#include "Conductor_Waves.h"
#include "Kismet/GameplayStatics.h"


ASpawner_Wave::ASpawner_Wave()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawner_Wave::BeginPlay()
{
	Super::BeginPlay();

	RNG.Initialize(RandomSeed);
	
	if (bUseMusicConductor)
	{
		// Try to find Conductor_Waves in the level
		AConductor_Waves* Conductor = Cast<AConductor_Waves>(
			UGameplayStatics::GetActorOfClass(this, AConductor_Waves::StaticClass())
		);

		if (Conductor)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawner %s bound to Conductor %s"),
				*GetName(), *Conductor->GetName());

			Conductor->OnBeat.AddDynamic(this, &ASpawner_Wave::HandleBeat);
			return; // do NOT start our own timer if conductor exists
		}
	}

	// Fallback: use old timer-based spawning
	UE_LOG(LogTemp, Log, TEXT("Spawner %s using internal timer (no Conductor or disabled)."),
		*GetName());

	StartSpawning();
}

void ASpawner_Wave::HandleBeat(int32 BeatIndex)
{
	// EXAMPLE PATTERN LOGIC:
	// For now, spawn on EVERY beat.

	// You can easily adjust patterns:
	// - every 2nd beat: if (BeatIndex % 2 == 0)
	// - burst at start of bar: use OnBar instead, etc.

	SpawnOnce();
}



void ASpawner_Wave::StartSpawning()
{
	const float FirstDelay = ComputeInterval();
	GetWorldTimerManager().SetTimer(TimerHandle_Spawn, this, &ASpawner_Wave::SpawnOnce, FirstDelay, false);
}

float ASpawner_Wave::ComputeInterval() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const AGS_Waves* GS = World->GetGameState<AGS_Waves>())
		{
			if (Difficulty)
			{
				return FMath::Max(0.05f, Difficulty->GetSpawnIntervalAt(GS->ElapsedSeconds, SpawnInterval));
			}
		}
	}
	return SpawnInterval;
}

void ASpawner_Wave::SpawnOnce()
{
	if (!WaveClass)
	{
		StartSpawning();
		return;
	}

	UWorld* World = GetWorld();
	if (!World) { return; }

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor_Wave* Wave = World->SpawnActor<AActor_Wave>(WaveClass, GetActorTransform(), Params);
	if (Wave)
	{
		// Random frequency 0..3
		const int32 FreqIdx = RNG.RandRange(0, 3);
		const EWaveFrequency NewFreq = static_cast<EWaveFrequency>(FreqIdx);

		Wave->SetFrequency(NewFreq);

		UE_LOG(LogTemp, Log, TEXT("Spawner %s created wave %s with Freq=%d"),
			*GetName(), *Wave->GetName(), FreqIdx);

		// Optional: speed multiplier from difficulty
		if (const AGS_Waves* GS = World->GetGameState<AGS_Waves>())
		{
			if (Difficulty)
			{
				const float Mul = Difficulty->GetWaveSpeedMulAt(GS->ElapsedSeconds, 1.0f);
				Wave->Spec.Speed *= Mul;
			}
		}
	}

	// schedule next
	const float NextInterval = ComputeInterval();
	GetWorldTimerManager().SetTimer(TimerHandle_Spawn, this, &ASpawner_Wave::SpawnOnce, NextInterval, false);
}

