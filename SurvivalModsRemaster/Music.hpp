#pragma once
#include "natives.h"
#include "Calculations.hpp"
#include <string>

namespace MUSIC
{
	enum class TrackGroups : int
	{
		SurvivalPrimary,
		BusinessBattle,
		BikerContracts,
		Casino,
		Casino2,
		Tuner,
		Hacker,
		SurvivalZombies
	};

	struct MusicData
	{
		static TrackGroups currentTrackGroup;
		static bool finalWaveHI;
	};

	void PrepareTracks(int forced = -1);
	void StopTrack();
	void StartTrack();
	void LowIntensityTrack();
	void Process(int enemyCount, int maxCount);
	void MissionCompletedSound();
	void FinalWaveMusic();
	void FinalWaveMusicHighIntensity();
}
