#pragma once

#include "natives.h"
#include "Calculations.hpp"
#include "SurvivalTypes.hpp"
#include "Jesus.hpp"
#include "Screen.hpp"
#include "SurvivalSpawner.hpp"
#include "Survival.hpp"
#include "Script.hpp"
#include "Music.hpp"
#include "Intermission.hpp"
#include "MissionTimers.hpp"
#include "Blip.hpp"
#include <vector>


namespace ENEMIES
{
	struct EnemiesData
	{
		static int currentAircraft;
		static int currentVehicles;
		static bool canSpawnMore;
		static bool limitReached;
		static int currentWaveSize;
		static int currentDogCount;
		static int currentJugCount;
		static int kills;
	};

    void Process();
    void RemoveUnusedVehicles();
	void ResetCounters();
	void ClearVectors();
}
