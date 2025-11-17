// Source/Waves/Public/Conductor_Waves.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Conductor_Waves.generated.h"

class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeat, int32, BeatIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBar, int32, BarIndex, int32, BeatInBar);

/**
 * Simple music conductor that:
 * - Plays a music track
 * - Fires OnBeat / OnBar events based on BPM
 */
UCLASS()
class WAVES_API AConductor_Waves : public AActor
{
	GENERATED_BODY()

public:
	AConductor_Waves();

	/** The music track to play at BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music")
	TObjectPtr<USoundBase> MusicTrack;

	/** BPM (beats per minute) of the track */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (ClampMin = "40.0", ClampMax = "200.0"))
	float BPM = 120.f;

	/** Beats per bar (time signature numerator, e.g. 4 for 4/4) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (ClampMin = "1", ClampMax = "16"))
	int32 BeatsPerBar = 4;

	/** Fired every beat: 0,1,2,... */
	UPROPERTY(BlueprintAssignable, Category = "Music")
	FOnBeat OnBeat;

	/** Fired every beat with bar info */
	UPROPERTY(BlueprintAssignable, Category = "Music")
	FOnBar OnBar;

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle_Beat;

	int32 BeatCounter = 0;
	int32 BarCounter = 0;

	void HandleBeat();
};
