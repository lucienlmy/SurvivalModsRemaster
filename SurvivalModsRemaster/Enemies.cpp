#include <main.h>
#include "pch.h"
#include "Enemies.hpp"

int ENEMIES::EnemiesData::currentAircraft;
int ENEMIES::EnemiesData::currentVehicles;
bool ENEMIES::EnemiesData::canSpawnMore;
bool ENEMIES::EnemiesData::limitReached;
int ENEMIES::EnemiesData::currentWaveSize;
int ENEMIES::EnemiesData::kills;
std::vector<Enemy> footEnemies;
std::vector<Vehicle> enemyVehicles;
std::vector<Enemy> deadEnemies;
JESUS::Jesus enemyJesus;
bool jesusSpawned;
bool playerOnFireByFiery = false;
int lastTankCheck = 0;
int lastExplosiveCheck = 0;
int lastBeastCheck = 0;

std::vector<Hash> alienWeapons =
{
    0x476BF155,
    0xB62D1F67
};

void ENEMIES::ResetCounters()
{
    EnemiesData::currentAircraft = 0;
    EnemiesData::currentVehicles = 0;
    EnemiesData::canSpawnMore = true;
    EnemiesData::limitReached = false;
    jesusSpawned = false;
    EnemiesData::currentWaveSize = 0;
    EnemiesData::kills = 0;
}

void ENEMIES::ClearVectors()
{
    for (Enemy enemy : footEnemies)
    {
        Blip blip = HUD::GET_BLIP_FROM_ENTITY(enemy.ped);
        
        if (blip != NULL)
        {
            HUD::REMOVE_BLIP(&blip);
        }

        ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&enemy.ped);
    }

    for (Vehicle vehicle : enemyVehicles)
    {
        ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&vehicle);
    }

    for (Enemy deadEnemy : deadEnemies)
    {
        ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&deadEnemy.ped);
    }

    footEnemies.clear();
    enemyVehicles.clear();
    deadEnemies.clear();
}

int GetKillTime(Ped ped)
{
    Entity killer = PED::GET_PED_SOURCE_OF_DEATH(ped);

    if (killer != PLAYER::PLAYER_PED_ID())
    {
        return 0;
    }

    Hash model = ENTITY::GET_ENTITY_MODEL(ped);

    switch (model)
    {
    case 0x90EF5134:
        SCREEN::ShowNotification("Juggernaut: ~o~+20 seconds");
        return 15000;
    case 0xCE2CB751:
        SCREEN::ShowNotification("Jesus: ~o~+15 seconds");
        return 15000;
    case 0xAD340F5A:
    case 0x61D4C771:
        if (PED::GET_PED_MAX_HEALTH(ped) == 1250)
        {
            SCREEN::ShowNotification("Ragemode Sasquatch: ~o~+15 seconds");
            return 15000;
        }
        else
        {
            break;
        }
    case 0x9563221D:
        SCREEN::ShowNotification("Rottweiler: ~o~+8 seconds");
        return 8000;
    default:
        break;
    }

    if (FIRE::IS_ENTITY_ON_FIRE(ped))
    {
        SCREEN::ShowNotification("Burn!: ~r~+4 seconds");
        return 4000;
    }

    int lastDamagedBone;
    PED::GET_PED_LAST_DAMAGE_BONE(ped, &lastDamagedBone);

    switch (lastDamagedBone)
    {
        case eBone::SKEL_Head:
            SCREEN::ShowNotification("Headshot: ~g~+5 seconds");
            return 5000;
        case eBone::SKEL_Spine_Root:
        case eBone::SKEL_Spine0:
        case eBone::SKEL_Spine1:
        case eBone::SKEL_Spine2:
        case eBone::SKEL_Spine3:
            SCREEN::ShowNotification("Torso: ~y~+2.5 seconds");
            return 2500;
        case eBone::SKEL_L_UpperArm:
        case eBone::SKEL_R_UpperArm:
        case eBone::SKEL_L_Forearm:
        case eBone::SKEL_R_Forearm:
        case eBone::SKEL_L_Thigh:
        case eBone::SKEL_R_Thigh:
        case eBone::SKEL_L_Calf:
        case eBone::SKEL_R_Calf:
            SCREEN::ShowNotification("Limb: ~r~+1.25 seconds");
            return 1250;
        default:
            SCREEN::ShowNotification("Other: ~c~+1 second");
            return 1000;
    }
}

void RemoveDeadEnemies()
{
    for (size_t i = 0; i < footEnemies.size(); i++)
    {
        Enemy body = footEnemies.at(i);

        if (!PED::IS_PED_DEAD_OR_DYING(body.ped, 1))
        {
            if (PED::IS_PED_FLEEING(body.ped) && !CALC::IsInRange_2(ENTITY::GET_ENTITY_COORDS(body.ped, true), SURVIVAL::SpawnerData::location, 160))
            {
                ENTITY::SET_ENTITY_HEALTH(body.ped, 0, 0, NULL);
            }
            else
            {
                continue;
            }
        }

        bool isJesus = ENTITY::GET_ENTITY_MODEL(body.ped) == 0xCE2CB751;

        if (SURVIVAL::SurvivalData::timed)
        {
            SURVIVAL::SurvivalData::timedTimeLeft += GetKillTime(body.ped);
        }

        if (jesusSpawned && isJesus)
        {
            jesusSpawned = false;
        }

        if (!PED::CAN_PED_RAGDOLL(body.ped))
        {
            PED::SET_PED_CAN_RAGDOLL(body.ped, true);
        }

        Blip blip = HUD::GET_BLIP_FROM_ENTITY(body.ped);

        if (HUD::DOES_BLIP_EXIST(blip))
        {
            HUD::REMOVE_BLIP(&blip);
        }

        if (!jesusSpawned)
        {
            ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&body.ped);
            ENEMIES::EnemiesData::kills += 1;
        }
        else
        {
            int index;

            if (body.enemyType == eEnemyType::Explosive)
            {
                if (!body.exploded)
                    continue;

                ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&body.ped);
                ENEMIES::EnemiesData::kills += 1;
            } 
            else
            {
                deadEnemies.push_back(body);
            }
        }

        footEnemies.erase(footEnemies.begin() + i);
        break;
    }

    if (!jesusSpawned && !deadEnemies.empty())
    {
        for (Enemy body : deadEnemies)
        {
            ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&body.ped);
            ENEMIES::EnemiesData::kills += 1;
        }

        deadEnemies.clear();
    }
}

void ENEMIES::RemoveUnusedVehicles()
{
    bool vehicleDestroyed;
    int vehiclePassengers;

    for (size_t i = 0; i < enemyVehicles.size(); i++)
    {
        vehicleDestroyed = ENTITY::IS_ENTITY_DEAD(enemyVehicles.at(i), 1);
        vehiclePassengers = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(enemyVehicles.at(i), true, false);

        if (vehicleDestroyed || vehiclePassengers == 0)
        {
            BLIPS::DeleteBlipForEntity(enemyVehicles.at(i));

            if (vehicleDestroyed && SURVIVAL::SurvivalData::timed)
            {
                SURVIVAL::SurvivalData::timedTimeLeft += 10000;
                SCREEN::ShowNotification("Vehicle: ~p~+10 seconds");
            }

            ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&enemyVehicles.at(i));
            enemyVehicles.erase(enemyVehicles.begin() + i);
            return;
        }
        else if (vehiclePassengers > 0)
        {
            Ped ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(enemyVehicles.at(i), -1, 0);
            if (PED::IS_PED_DEAD_OR_DYING(ped, 1))
            {
                BLIPS::DeleteBlipForEntity(enemyVehicles.at(i));
                ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&enemyVehicles.at(i));
                enemyVehicles.erase(enemyVehicles.begin() + i);
                return;
            }
        }
    }
}

void ProcessSuicidals()
{
    for (Enemy &enemy : footEnemies)
    {
        if (enemy.enemyType != eEnemyType::Explosive)
        {
            continue;
        }

        if (PED::IS_PED_DEAD_OR_DYING(enemy.ped, 1) && !enemy.exploded)
        {
            if (!enemy.timer)
            {
                int lastDamagedBone;
                PED::GET_PED_LAST_DAMAGE_BONE(enemy.ped, &lastDamagedBone);

                if (lastDamagedBone == eBone::SKEL_Head)
                {
                    Vector3 coords = ENTITY::GET_ENTITY_COORDS(enemy.ped, true);
                    FIRE::ADD_EXPLOSION(coords.x, coords.y, coords.z, eExplosionType::ExplosionTypeStickyBomb, 5.0f, true, false, 2.0f, false);
                    enemy.exploded = true;
                    continue;
                }

                enemy.deadCoords = ENTITY::GET_ENTITY_COORDS(enemy.ped, true);
                enemy.timeOfDeath = MISC::GET_GAME_TIMER();
                enemy.timer = true;
            } else
            {
                if (MISC::GET_GAME_TIMER() - enemy.timeOfDeath >= 2000)
                {
                    FIRE::ADD_EXPLOSION(enemy.deadCoords.x, enemy.deadCoords.y, enemy.deadCoords.z , eExplosionType::ExplosionTypeStickyBomb, 5.0f, true, false, 2.0f, false);
                    enemy.exploded = true;
                }
            }
        }
        else
        {
            Vector3 coords = ENTITY::GET_ENTITY_COORDS(enemy.ped, true);
            Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);

            if (CALC::IsInRange_2(coords, playerCoords, 2.0f))
            {
                FIRE::ADD_EXPLOSION(coords.x, coords.y, coords.z, eExplosionType::ExplosionTypeStickyBomb, 5.0f, true, false, 2.0f, false);
                enemy.exploded = true;
            }
        }
    }
}

std::vector<Hash> GetWeapons(Hash pedModel)
{
    if (pedModel == 0x64611296)
    {
        return alienWeapons;
    }

    if (SURVIVAL::SurvivalData::hardcore)
        return SURVIVAL::SpawnerData::strongWeapons;

    switch (SURVIVAL::SurvivalData::CurrentWave)
    {
        case 1:
        case 2:
        case 3:
            return SURVIVAL::SpawnerData::weakWeapons;
        case 4:
        case 5:
        case 6:
        case 7:
            return SURVIVAL::SpawnerData::medWeapons;
        default:
            return SURVIVAL::SpawnerData::strongWeapons;
    }
}

void InitializeJesus(Ped ped)
{
    enemyJesus = JESUS::Jesus(ped);
    jesusSpawned = true;
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
    PED::SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
    PED::SET_PED_CAN_RAGDOLL(ped, false);
    PED::SET_PED_CONFIG_FLAG(ped, 281, false);
    PED::SET_PED_MAX_HEALTH(ped, 1000);
    ENTITY::SET_ENTITY_HEALTH(ped, 1000, 0, NULL);
    BLIPS::CreateForEnemyPed(ped, 305, "Jesus Christ");
}

void InitializeRageEnemy(Ped ped)
{
    PED::SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
    PED::SET_PED_CAN_RAGDOLL(ped, false);
    PED::SET_PED_CONFIG_FLAG(ped, 281, true);
    PED::SET_PED_MAX_HEALTH(ped, 1250);
    ENTITY::SET_ENTITY_HEALTH(ped, 1250, 0, NULL);
    TASK::TASK_PUT_PED_DIRECTLY_INTO_MELEE(ped, PLAYER::PLAYER_PED_ID(), 0, -1, 0, 0);
    PED::SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(100);
    BLIPS::CreateForEnemyPed(ped, 671, "Ragemode Sasquatch");
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
}

const char* GetSpeechParam(int paramType) {
    switch (paramType)
    {
    case 0:
        return "SPEECH_PARAMS_STANDARD";

    case 1:
        return "SPEECH_PARAMS_ALLOW_REPEAT";

    case 2:
        return "SPEECH_PARAMS_BEAT";

    case 3:
        return "SPEECH_PARAMS_FORCE";

    case 4:
        return "SPEECH_PARAMS_FORCE_FRONTEND";

    case 5:
        return "SPEECH_PARAMS_FORCE_NO_REPEAT_FRONTEND";

    case 6:
        return "SPEECH_PARAMS_FORCE_NORMAL";

    case 7:
        return "SPEECH_PARAMS_FORCE_NORMAL_CLEAR";

    case 8:
        return "SPEECH_PARAMS_FORCE_NORMAL_CRITICAL";

    case 9:
        return "SPEECH_PARAMS_FORCE_SHOUTED";

    case 10:
        return "SPEECH_PARAMS_FORCE_SHOUTED_CLEAR";

    case 11:
        return "SPEECH_PARAMS_FORCE_SHOUTED_CRITICAL";

    case 12:
        return "SPEECH_PARAMS_FORCE_PRELOAD_ONLY";

    case 13:
        return "SPEECH_PARAMS_MEGAPHONE";

    case 14:
        return "SPEECH_PARAMS_HELI";

    case 15:
        return "SPEECH_PARAMS_FORCE_MEGAPHONE";

    case 16:
        return "SPEECH_PARAMS_FORCE_HELI";

    case 17:
        return "SPEECH_PARAMS_INTERRUPT";

    case 18:
        return "SPEECH_PARAMS_INTERRUPT_SHOUTED";

    case 19:
        return "SPEECH_PARAMS_INTERRUPT_SHOUTED_CLEAR";

    case 20:
        return "SPEECH_PARAMS_INTERRUPT_SHOUTED_CRITICAL";

    case 21:
        return "SPEECH_PARAMS_INTERRUPT_NO_FORCE";

    case 22:
        return "SPEECH_PARAMS_INTERRUPT_FRONTEND";

    case 23:
        return "SPEECH_PARAMS_INTERRUPT_NO_FORCE_FRONTEND";

    case 24:
        return "SPEECH_PARAMS_ADD_BLIP";

    case 25:
        return "SPEECH_PARAMS_ADD_BLIP_ALLOW_REPEAT";

    case 26:
        return "SPEECH_PARAMS_ADD_BLIP_FORCE";

    case 27:
        return "SPEECH_PARAMS_ADD_BLIP_SHOUTED";

    case 28:
        return "SPEECH_PARAMS_ADD_BLIP_SHOUTED_FORCE";

    case 29:
        return "SPEECH_PARAMS_ADD_BLIP_INTERRUPT";

    case 30:
        return "SPEECH_PARAMS_ADD_BLIP_INTERRUPT_FORCE";

    case 31:
        return "SPEECH_PARAMS_FORCE_PRELOAD_ONLY_SHOUTED";

    case 32:
        return "SPEECH_PARAMS_FORCE_PRELOAD_ONLY_SHOUTED_CLEAR";

    case 33:
        return "SPEECH_PARAMS_FORCE_PRELOAD_ONLY_SHOUTED_CRITICAL";

    case 34:
        return "SPEECH_PARAMS_SHOUTED";

    case 35:
        return "SPEECH_PARAMS_SHOUTED_CLEAR";

    case 36:
        return "SPEECH_PARAMS_SHOUTED_CRITICAL";

    default:
        return "SPEECH_PARAMS_STANDARD";
    }
}

void PlayZombieSound(Ped zombie, int soundType, int speechParam) {
    const char* soundName;

    switch (soundType) {
    case 0:
        soundName = "UNDEAD_SPAWN";
        break;
    case 1:
        soundName = "UNDEAD_SPAWN_FIRE";
        break;
    case 2:
        soundName = "UNDEAD_SPAWN_PLAGUE";
        break;
    case 3:
        soundName = "UNDEAD_SPAWN_SUCIDE";
        break;
    case 4:
        soundName = "UNDEAD_SPAWN_HEAVY";
        break;
    case 5:
        soundName = "UNDEAD_SPAWN_BOAR";
        break;
    case 6:
        soundName = "UNDEAD_SPAWN_DEER";
        break;
    case 7:
        soundName = "UNDEAD_SPAWN_DOG";
        break;
    case 8:
        soundName = "UNDEAD_WAR_CRY";
        break;
    case 9:
        soundName = "UNDEAD_EXPLODER_SPRINT";
        break;
    case 10:
        soundName = "UNDEAD_IDLE";
        break;
    default:
        soundName = "UNDEAD_SPAWN";
        break;
    }

    AUDIO::PLAY_PED_AMBIENT_SPEECH_WITH_VOICE_NATIVE(zombie, soundName, "YK_Survival_01", GetSpeechParam(speechParam), 1);
}

int FieryEnemyCount()
{
    int count = 0;

    for (Enemy& enemy : footEnemies)
    {
        if (enemy.fiery)
        {
            count += 1;
        }
    }

    return count;
}

int FieryEnemyLimit()
{
    if (SURVIVAL::SurvivalData::CurrentWave <= 7)
    {
        return 1;
    }

    return 2;
}

bool ZombieResourcesLoaded()
{
    return STREAMING::HAS_ANIM_DICT_LOADED("anim@scripted@surv@ig2_zombie_spawn@shambler@") &&
        STREAMING::HAS_ANIM_DICT_LOADED("anim@scripted@surv@ig2_zombie_spawn@runner@") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@move_m@zombie@core") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@move_m@zombie@strafe") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@melee@unarmed@streamed_core_zombie") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@melee@unarmed@streamed_variations_zombie") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@melee@unarmed@streamed_taunts_zombie");
}

void InitializeSuicidal(Ped ped)
{
    PED::SET_PED_MAX_HEALTH(ped, 420);
    ENTITY::SET_ENTITY_HEALTH(ped, 420, 0, NULL);
    PED::SET_PED_CONFIG_FLAG(ped, 281, true);
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);

    if (!SURVIVAL::SurvivalData::zombies)
    {
        WEAPON::GIVE_WEAPON_TO_PED(ped, eWeapon::WeaponKnife, 1, true, true);
        WEAPON::GIVE_WEAPON_TO_PED(ped, eWeapon::WeaponHatchet, 1, true, true);
        WEAPON::GIVE_WEAPON_TO_PED(ped, eWeapon::WeaponHammer, 1, true, true);
        BLIPS::CreateForEnemyPed(ped, 486, "Suicide Bomber");
        TASK::TASK_PUT_PED_DIRECTLY_INTO_MELEE(ped, PLAYER::PLAYER_PED_ID(), 0, -1, 0, 0);
    }
    else
    {
        WEAPON::REMOVE_ALL_PED_WEAPONS(ped, true);

        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 0, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 13, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 50, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 58, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 38, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 1, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 17, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 42, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 71, true);
        PED::SET_PED_CONFIG_FLAG(ped, 430, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 28, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 4, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 21, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(ped, 31, false);
        PED::SET_PED_CONFIG_FLAG(ped, 231, true);

        PED::SET_PED_COMBAT_RANGE(ped, 0);
        PED::SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(2);
        PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
        PED::SET_PED_CAN_SWITCH_WEAPON(ped, false);
        PED::SET_PED_COMBAT_ABILITY(ped, 2);
        PED::SET_PED_MONEY(ped, 0);

        AUDIO::DISABLE_PED_PAIN_AUDIO(ped, true);

        while (!ZombieResourcesLoaded())
        {
            WAIT(50);
        }

        if (!AUDIO::IS_AUDIO_SCENE_ACTIVE("DLC_24-1_YK_Mixer_Scene"))
        {
            AUDIO::START_AUDIO_SCENE("DLC_24-1_YK_Mixer_Scene");
        }

        PED::SET_PED_MOVEMENT_CLIPSET(ped, "clipset@anim@ingame@move_m@zombie@core", 0.25f);
        PED::SET_PED_USING_ACTION_MODE(ped, true, -1, "clipset@anim@ingame@move_m@zombie@core");
        PED::SET_PED_STRAFE_CLIPSET(ped, "clipset@anim@ingame@move_m@zombie@strafe");
        WEAPON::SET_WEAPON_ANIMATION_OVERRIDE(ped, MISC::GET_HASH_KEY("ZOMBIE"));
        PED::SET_PED_MIN_MOVE_BLEND_RATIO(ped, 3);
        PED::SET_PED_MAX_MOVE_BLEND_RATIO(ped, 3);

        static const char* const ANIMS[] = {
       "action_01",
       "action_02",
       "action_03",
       "action_04"
        };

        const char* anim_name = ANIMS[CALC::RanInt(4, 1) - 1];
        const char* anim_dict = "anim@scripted@surv@ig2_zombie_spawn@runner@";

        PlayZombieSound(ped, 0, 10);
        PlayZombieSound(ped, 8, 10);

        TASK::CLEAR_PED_TASKS(ped);

        int sequence = -1;
        TASK::OPEN_SEQUENCE_TASK(&sequence);

        TASK::TASK_PLAY_ANIM(0, anim_dict, anim_name, 1000.0f, -8.0f, -1, 8, 0.0f, false, false, false);
        TASK::TASK_COMBAT_PED(0, PLAYER::PLAYER_PED_ID(), 67108864, 16);

        TASK::CLOSE_SEQUENCE_TASK(sequence);
        TASK::TASK_PERFORM_SEQUENCE(ped, sequence);

        TASK::CLEAR_SEQUENCE_TASK(&sequence);
    }
}

void InitializeAnimal(Ped ped)
{
    if (!SURVIVAL::SurvivalData::zombies)
    {
        BLIPS::CreateForEnemyPed(ped, 463, "Enemy Animal");
    }
    else
    {
        Hash model = ENTITY::GET_ENTITY_MODEL(ped);

        switch (model)
        {
        case 0x8B297EF4:
            PED::SET_PED_MAX_HEALTH(ped, 500);
            ENTITY::SET_ENTITY_HEALTH(ped, 500, 0, NULL);
            PlayZombieSound(ped, 7, 10);
            break;
        case 0xAA4B67D0:
            PED::SET_PED_MAX_HEALTH(ped, 350);
            ENTITY::SET_ENTITY_HEALTH(ped, 350, 0, NULL);
            PlayZombieSound(ped, 6, 10);
            break;
        }
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, MISC::GET_HASH_KEY("COUGAR"));
    TASK::TASK_PUT_PED_DIRECTLY_INTO_MELEE(ped, PLAYER::PLAYER_PED_ID(), 0, 0, 0, 0);
}

void InitializeJuggernaut(Enemy& enemy)
{
    PED::SET_PED_SUFFERS_CRITICAL_HITS(enemy.ped, false);
    PED::SET_PED_CAN_RAGDOLL(enemy.ped, false);
    PED::SET_PED_CONFIG_FLAG(enemy.ped, 281, true);
    PED::SET_PED_MAX_HEALTH(enemy.ped, 1000);
    PED::SET_PED_COMBAT_MOVEMENT(enemy.ped, 2);
    ENTITY::SET_ENTITY_HEALTH(enemy.ped, 1000, 0, NULL);
    PED::SET_PED_ARMOUR(enemy.ped, SURVIVAL::SurvivalData::hardcore ? 300 : 150);
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(enemy.ped, Data::enemiesRelGroup);

    if (SURVIVAL::SurvivalData::zombies)
    {
        bool fieryChance = SURVIVAL::SurvivalData::CurrentWave >= 6 && FieryEnemyCount() < FieryEnemyLimit() && MISC::GET_RANDOM_INT_IN_RANGE(0, 100) <= 5;

        WEAPON::REMOVE_ALL_PED_WEAPONS(enemy.ped, true);

        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 0, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 5, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 46, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 13, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 50, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 58, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 38, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 1, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 3, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 17, false);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 42, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 71, true);
        PED::SET_PED_CONFIG_FLAG(enemy.ped, 430, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 28, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 4, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 21, true);
        PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 31, false);
        PED::SET_PED_CONFIG_FLAG(enemy.ped, 231, true);

        PED::SET_PED_COMBAT_RANGE(enemy.ped, 0);
        PED::SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(2);
        PED::SET_PED_COMBAT_MOVEMENT(enemy.ped, 2);
        PED::SET_PED_CAN_SWITCH_WEAPON(enemy.ped, false);
        PED::SET_PED_COMBAT_ABILITY(enemy.ped, 2);
        PED::SET_PED_MONEY(enemy.ped, 0);

        AUDIO::DISABLE_PED_PAIN_AUDIO(enemy.ped, true);

        if (fieryChance)
        {
            enemy.fiery = true;
            PED::SET_PED_CONFIG_FLAG(enemy.ped, 109, true);
            PED::SET_PED_CONFIG_FLAG(enemy.ped, 118, false);
            FIRE::START_ENTITY_FIRE(enemy.ped);
        }

        while (!ZombieResourcesLoaded())
        {
            WAIT(50);
        }

        PED::SET_PED_MOVEMENT_CLIPSET(enemy.ped, "clipset@anim@ingame@move_m@zombie@core", 0.25f);
        PED::SET_PED_USING_ACTION_MODE(enemy.ped, true, -1, "clipset@anim@ingame@move_m@zombie@core");
        PED::SET_PED_STRAFE_CLIPSET(enemy.ped, "clipset@anim@ingame@move_m@zombie@strafe");
        WEAPON::SET_WEAPON_ANIMATION_OVERRIDE(enemy.ped, MISC::GET_HASH_KEY("ZOMBIE"));
        PED::SET_PED_MIN_MOVE_BLEND_RATIO(enemy.ped, 1);
        PED::SET_PED_MAX_MOVE_BLEND_RATIO(enemy.ped, 1);

        static const char* const ANIMS[] = {
       "action_01",
       "action_02",
       "action_03",
       "action_04"
        };

        const char* anim_name = ANIMS[CALC::RanInt(4, 1) - 1];
        const char* anim_dict = "anim@scripted@surv@ig2_zombie_spawn@shambler@";

        PlayZombieSound(enemy.ped, 0, 10);
        PlayZombieSound(enemy.ped, 8, 10);

        TASK::CLEAR_PED_TASKS(enemy.ped);

        int sequence = -1;
        TASK::OPEN_SEQUENCE_TASK(&sequence);

        TASK::TASK_PLAY_ANIM(0, anim_dict, anim_name, 1000.0f, -8.0f, -1, 8, 0.0f, false, false, false);
        TASK::TASK_COMBAT_PED(0, PLAYER::PLAYER_PED_ID(), 67108864, 16);

        TASK::CLOSE_SEQUENCE_TASK(sequence);
        TASK::TASK_PERFORM_SEQUENCE(enemy.ped, sequence);

        TASK::CLEAR_SEQUENCE_TASK(&sequence);
    }
    else
    {
        BLIPS::CreateForEnemyPed(enemy.ped, 543, "Enemy Juggernaut");
        int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

        if (SURVIVAL::SurvivalData::hardcore)
        {
            accuracyModifier = 10;
        }

        PED::SET_PED_ACCURACY(enemy.ped, accuracyModifier * 3);
        WEAPON::GIVE_WEAPON_TO_PED(enemy.ped, eWeapon::WeaponMinigun, 9000, true, true);
        PED::SET_PED_FIRING_PATTERN(enemy.ped, eFiringPattern::FiringPatternFullAuto);
        TASK::TASK_COMBAT_PED(enemy.ped, PLAYER::PLAYER_PED_ID(), 0, 16);
    }
}

void InitializeZombie(Enemy& enemy, bool fast)
{
    bool fieryChance = SURVIVAL::SurvivalData::CurrentWave >= 6 && FieryEnemyCount() < FieryEnemyLimit() && MISC::GET_RANDOM_INT_IN_RANGE(0, 100) <= 15;
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(enemy.ped, Data::enemiesRelGroup);

    WEAPON::REMOVE_ALL_PED_WEAPONS(enemy.ped, true);

    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 0, false);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 5, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 46, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 13, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 50, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 58, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 38, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 1, false);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 3, false);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 17, false);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 42, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 71, true);
    PED::SET_PED_CONFIG_FLAG(enemy.ped, 430, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 28, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 4, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 21, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(enemy.ped, 31, false);
    PED::SET_PED_CONFIG_FLAG(enemy.ped, 231, true);
    PED::SET_PED_CONFIG_FLAG(enemy.ped, 281, true);

    PED::SET_PED_COMBAT_RANGE(enemy.ped, 0);
    PED::SET_PED_COMBAT_MOVEMENT(enemy.ped, 2);
    PED::SET_PED_CAN_SWITCH_WEAPON(enemy.ped, false);
    PED::SET_PED_COMBAT_ABILITY(enemy.ped, 2);
    PED::SET_PED_MONEY(enemy.ped, 0);

    PED::SET_PED_ARMOUR(enemy.ped, 5 * SURVIVAL::SurvivalData::CurrentWave);
   
    AUDIO::DISABLE_PED_PAIN_AUDIO(enemy.ped, true);

    if (fieryChance)
    {
        enemy.fiery = true;
        PED::SET_PED_CONFIG_FLAG(enemy.ped, 109, true);
        PED::SET_PED_CONFIG_FLAG(enemy.ped, 118, false);
        FIRE::START_ENTITY_FIRE(enemy.ped);
        PED::SET_PED_MAX_HEALTH(enemy.ped, 500);
        ENTITY::SET_ENTITY_HEALTH(enemy.ped, 500, 0, NULL);
        PED::SET_PED_ARMOUR(enemy.ped, 70);
    }

    while (!ZombieResourcesLoaded())
    {
        WAIT(50);
    }

    PED::SET_PED_MOVEMENT_CLIPSET(enemy.ped, "clipset@anim@ingame@move_m@zombie@core", 0.25f);
    PED::SET_PED_USING_ACTION_MODE(enemy.ped, true, -1, "clipset@anim@ingame@move_m@zombie@core");
    PED::SET_PED_STRAFE_CLIPSET(enemy.ped, "clipset@anim@ingame@move_m@zombie@strafe");
    WEAPON::SET_WEAPON_ANIMATION_OVERRIDE(enemy.ped, MISC::GET_HASH_KEY("ZOMBIE"));

    if (fast)
    {
        PED::SET_PED_MIN_MOVE_BLEND_RATIO(enemy.ped, 1.0f);
        PED::SET_PED_MAX_MOVE_BLEND_RATIO(enemy.ped, 3.0f);
    }
    else
    {
        PED::SET_PED_MIN_MOVE_BLEND_RATIO(enemy.ped, 1);
        PED::SET_PED_MAX_MOVE_BLEND_RATIO(enemy.ped, 1.5f);
    }

    static const char* const ANIMS[] = {
    "action_01",
    "action_02",
    "action_03",
    "action_04"
    };

    const char* anim_name = ANIMS[CALC::RanInt(4, 1) - 1];
    const char* anim_dict = fast ? "anim@scripted@surv@ig2_zombie_spawn@runner@" : "anim@scripted@surv@ig2_zombie_spawn@shambler@";

    PlayZombieSound(enemy.ped, 0, 10);
    PlayZombieSound(enemy.ped, 8, 10);

    TASK::CLEAR_PED_TASKS(enemy.ped);

    int sequence = -1;
    TASK::OPEN_SEQUENCE_TASK(&sequence);

    TASK::TASK_PLAY_ANIM(0, anim_dict, anim_name, 1000.0f, -8.0f, -1, 8, 0.0f, false, false, false);
    TASK::TASK_COMBAT_PED(0, PLAYER::PLAYER_PED_ID(), 67108864, 16);

    TASK::CLOSE_SEQUENCE_TASK(sequence);
    TASK::TASK_PERFORM_SEQUENCE(enemy.ped, sequence);

    TASK::CLEAR_SEQUENCE_TASK(&sequence);
}

bool ZombieResourcesLoaded()
{
    return STREAMING::HAS_ANIM_DICT_LOADED("anim@scripted@surv@ig2_zombie_spawn@shambler@") &&
        STREAMING::HAS_ANIM_DICT_LOADED("anim@scripted@surv@ig2_zombie_spawn@runner@") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@move_m@zombie@core") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@move_m@zombie@strafe") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@melee@unarmed@streamed_core_zombie") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@melee@unarmed@streamed_variations_zombie") &&
        STREAMING::HAS_CLIP_SET_LOADED("clipset@anim@ingame@melee@unarmed@streamed_taunts_zombie");
}

void InitializeZombie(Ped ped, bool fast)
{
    while (!ZombieResourcesLoaded())
    {
        WAIT(50);
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    PED::SET_PED_MOVEMENT_CLIPSET(ped, "clipset@anim@ingame@move_m@zombie@core", 0.25f);
    PED::SET_PED_USING_ACTION_MODE(ped, true, -1, "clipset@anim@ingame@move_m@zombie@core");
    PED::SET_PED_STRAFE_CLIPSET(ped, "clipset@anim@ingame@move_m@zombie@strafe");
    WEAPON::SET_WEAPON_ANIMATION_OVERRIDE(ped, MISC::GET_HASH_KEY("ZOMBIE"));

    if (fast)
    {
        PED::SET_PED_MIN_MOVE_BLEND_RATIO(ped, 1.0f);
        PED::SET_PED_MAX_MOVE_BLEND_RATIO(ped, 3.0f);
    }
    else
    {
        PED::SET_PED_MIN_MOVE_BLEND_RATIO(ped, 0.25f);
        PED::SET_PED_MAX_MOVE_BLEND_RATIO(ped, 0.5f);
    }

    WEAPON::REMOVE_ALL_PED_WEAPONS(ped, true);

    PED::SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
    PED::SET_PED_COMBAT_ATTRIBUTES(ped, 1, false);
    PED::SET_PED_COMBAT_RANGE(ped, 1);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    PED::SET_PED_CAN_SWITCH_WEAPON(ped, false);

    if (fast) {
        PED::SET_PED_COMBAT_ABILITY(ped, 2);

    }
    else {
        PED::SET_PED_COMBAT_ABILITY(ped, 1);
    }

void InitializeEnemy(Enemy& enemy)
{
    Hash pedModel = ENTITY::GET_ENTITY_MODEL(enemy.ped);

    if (SURVIVAL::SurvivalData::zombies)
    {
        InitializeZombie(enemy, SURVIVAL::SurvivalData::CurrentWave >= 5);
        return;
    }

    if (SURVIVAL::SurvivalData::zombies)
    {
        InitializeZombie(ped, SURVIVAL::SurvivalData::CurrentWave >= 5);
        return;
    }

    if (pedModel == 0xCE2CB751 &&  !jesusSpawned)
    {
        InitializeJesus(enemy.ped);
        return;
    }

    if ((pedModel == 0xAD340F5A || pedModel == 0x61D4C771))
    {
        int rageChance = CALC::RanInt(100, 1);

        if (rageChance <= SURVIVAL::SurvivalData::CurrentWave * 2)
        {
            InitializeRageEnemy(enemy.ped);
            return;
        }
    }

    switch (pedModel)
    {
        case 0x585C0B52:
        case 0x3AE4A33B:
        case 0x5CDEF405:
        case 0xEDBC7546:
        case 0x26F067AD:
        case 0x7B8B434B:
        case 0x5E3DA4A4:
        case 0x15F8700D:
        case 0x616C97B9:
        case 0x72C0CAD2:
        case 0x8D8F1B10:
            PED::SET_PED_CONFIG_FLAG(enemy.ped, 155, false);
            PED::SET_PED_CONFIG_FLAG(enemy.ped, 42, true);
            PED::SET_PED_CONFIG_FLAG(enemy.ped, 301, true);
            break;
        default:
            break;
    }

    if (pedModel == 0x64611296)
    {
        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(enemy.ped);
        AUDIO::DISABLE_PED_PAIN_AUDIO(enemy.ped, true);
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(enemy.ped, Data::enemiesRelGroup);
    int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

    if (SURVIVAL::SurvivalData::hardcore)
        accuracyModifier = 10;

    PED::SET_PED_ARMOUR(enemy.ped, SURVIVAL::SurvivalData::hardcore ? 150 : SURVIVAL::SurvivalData::CurrentWave * 10);
    PED::SET_PED_ACCURACY(enemy.ped, 5 + ceil(accuracyModifier * 1.2));
    std::vector<Hash> weapons = GetWeapons(pedModel);
    size_t index = CALC::RanInt(weapons.size() - (size_t)1, (size_t)0);
    Hash weaponHash = weapons.at(index);
    PED::SET_PED_CONFIG_FLAG(enemy.ped, 281, true);
    WEAPON::GIVE_WEAPON_TO_PED(enemy.ped, weaponHash, 1000, true, true);
    TASK::TASK_COMBAT_PED(enemy.ped, PLAYER::PLAYER_PED_ID(), 0, 16);
    BLIPS::CreateForEnemyPed(enemy.ped);
    PED::SET_PED_COMBAT_MOVEMENT(enemy.ped, 2);
    int i = CALC::RanInt(100, 1);

    if (i <= accuracyModifier * 3)
    {
        PED::SET_PED_FIRING_PATTERN(enemy.ped, eFiringPattern::FiringPatternFullAuto);
    }
}

void InitializeEnemyInAircraft(Ped ped, bool passenger)
{
    Hash pedModel = ENTITY::GET_ENTITY_MODEL(ped);

    switch (pedModel)
    {
        case 0x5E3DA4A4:
        case 0x15F8700D:
        case 0x616C97B9:
        case 0x72C0CAD2:
        case 0x8D8F1B10:
            PED::SET_PED_CONFIG_FLAG(ped, 155, false);
            PED::SET_PED_CONFIG_FLAG(ped, 42, true);
            PED::SET_PED_CONFIG_FLAG(ped, 301, true);
            break;
        default:
            break;
    }

    if (pedModel == 0x64611296)
    {
        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(ped);
        AUDIO::DISABLE_PED_PAIN_AUDIO(ped, true);
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

    if (SURVIVAL::SurvivalData::hardcore)
        accuracyModifier = 10;

    PED::SET_PED_ARMOUR(ped, SURVIVAL::SurvivalData::hardcore ? 150 : SURVIVAL::SurvivalData::CurrentWave * 10);
    PED::SET_PED_ACCURACY(ped, 5 + accuracyModifier);
    std::vector<DWORD> weapons = GetWeapons(pedModel);
    size_t index = CALC::RanInt(weapons.size() - (size_t)1, (size_t)0);
    DWORD weaponHash = weapons.at(index);
    PED::SET_PED_CONFIG_FLAG(ped, 281, true);
    WEAPON::GIVE_WEAPON_TO_PED(ped, weaponHash, 1000, true, true);

    if (passenger)
        TASK::TASK_SHOOT_AT_ENTITY(ped, PLAYER::PLAYER_PED_ID(), 18000000, eFiringPattern::FiringPatternBurstFire);
    else
        TASK::TASK_COMBAT_PED(ped, PLAYER::PLAYER_PED_ID(), 0, 16);

    BLIPS::CreateForEnemyPed(ped);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    int i = CALC::RanInt(100, 1);

    if (i <= accuracyModifier * 3)
    {
        PED::SET_PED_FIRING_PATTERN(ped, eFiringPattern::FiringPatternFullAuto);
    }
}

void InitializeEnemyInVehicle(Ped ped, bool passenger)
{
    Hash pedModel = ENTITY::GET_ENTITY_MODEL(ped);

    switch (pedModel)
    {
        case 0x5E3DA4A4:
        case 0x15F8700D:
        case 0x616C97B9:
        case 0x72C0CAD2:
        case 0x8D8F1B10:
            PED::SET_PED_CONFIG_FLAG(ped, 155, false);
            PED::SET_PED_CONFIG_FLAG(ped, 42, true);
            PED::SET_PED_CONFIG_FLAG(ped, 301, true);
            break;
        default:
            break;
    }

    if (pedModel == 0x64611296)
    {
        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(ped);
        AUDIO::DISABLE_PED_PAIN_AUDIO(ped, true);
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

    if (SURVIVAL::SurvivalData::hardcore)
        accuracyModifier = 10;

    PED::SET_PED_ARMOUR(ped, SURVIVAL::SurvivalData::hardcore ? 150 : SURVIVAL::SurvivalData::CurrentWave * 10);
    PED::SET_PED_ACCURACY(ped, 5 + accuracyModifier);
    std::vector<Hash> weapons = GetWeapons(pedModel);
    size_t index = CALC::RanInt(weapons.size() - (size_t)1, (size_t)0);
    Hash weaponHash = weapons.at(index);
    WEAPON::GIVE_WEAPON_TO_PED(ped, weaponHash, 1000, true, true);

    if (passenger)
        TASK::TASK_VEHICLE_SHOOT_AT_PED(ped, PLAYER::PLAYER_PED_ID(), 40.0f);
    else
        TASK::TASK_COMBAT_PED(ped, PLAYER::PLAYER_PED_ID(), 0, 16);

    BLIPS::CreateForEnemyPed(ped);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    int i = CALC::RanInt(100, 1);

    if (i <= accuracyModifier * 3)
    {
        PED::SET_PED_FIRING_PATTERN(ped, eFiringPattern::FiringPatternFullAuto);
    }
}

void ProcessJesus()
{
    if (jesusSpawned)
    {
        if (deadEnemies.empty() && !enemyJesus.waiting)
        {
            enemyJesus.StartWaiting();
        }
        else if (!deadEnemies.empty())
        {
            if (enemyJesus.waiting)
            {
                enemyJesus.SetTarget(deadEnemies.front());
                enemyJesus.MoveToTarget();
            }
            else if (enemyJesus.movingToPed && enemyJesus.IsInRange())
            {
                enemyJesus.StartReviving();
            }
            else if (enemyJesus.revivingPed && enemyJesus.CanRevive())
            {
                enemyJesus.ReviveTarget();
                WAIT(1);

                for (int n = 0; n < deadEnemies.size(); n++)
                {
                    if (deadEnemies[n].ped == enemyJesus.targetPed.ped)
                    {
                        deadEnemies.erase(deadEnemies.begin() + n);
                        break;
                    }
                }

                if (enemyJesus.targetPed.enemyType == eEnemyType::Juggernaut)
                {
                    InitializeJuggernaut(enemyJesus.targetPed);
                }
                else if (ENTITY::GET_ENTITY_MODEL(enemyJesus.targetPed.ped) == 0x9563221D)
                {
                    InitializeAnimal(enemyJesus.targetPed.ped);
                }
                else
                {
                    InitializeEnemy(enemyJesus.targetPed);
                }

                footEnemies.push_back(enemyJesus.targetPed);
            }
        }
    }
}

void CheckDamageEvent(Enemy& zombie) {
    int currentHealth = ENTITY::GET_ENTITY_HEALTH(zombie.ped);

    if (zombie.lastHealth != currentHealth)
    {
        Hash model = ENTITY::GET_ENTITY_MODEL(zombie.ped);
        zombie.lastHealth = currentHealth;

        if (ENTITY::IS_ENTITY_DEAD(zombie.ped, false))
        {
            if (!zombie.playedDeathSound)
            {
                zombie.playedDeathSound = true;
                Vector3 coords = ENTITY::GET_ENTITY_COORDS(zombie.ped, false);

                switch (model)
                {
                case 0x8B297EF4:
                    AUDIO::PLAY_SOUND_FROM_COORD(-1, "Boar_Death", coords.x, coords.y, coords.z, "DLC_24-1_YK_Survival_Sounds", false, 40, false);
                    break;
                case 0xAA4B67D0:
                    AUDIO::PLAY_SOUND_FROM_COORD(-1, "Deer_Death", coords.x, coords.y, coords.z, "DLC_24-1_YK_Survival_Sounds", false, 40, false);
                    break;
                default:
                    AUDIO::PLAY_SOUND_FROM_COORD(-1, "Undead_Death", coords.x, coords.y, coords.z, "DLC_24-1_YK_Survival_Sounds", false, 40, false);
                    break;
                }
            }

            if (zombie.fiery && !zombie.exploded)
            {
                Vector3 coords = ENTITY::GET_ENTITY_COORDS(zombie.ped, false);
                FIRE::ADD_EXPLOSION(coords.x, coords.y, coords.z, eExplosionType::ExplosionTypeMolotov, 1, true, false, 0, false);
                zombie.exploded = true;
            }
        }
        else if (!FIRE::IS_ENTITY_ON_FIRE(zombie.ped) && CALC::RanInt(100, 0) <= 40)
        {
            switch (model)
            {
            case 0x8B297EF4:
                AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "Boar_Pain", zombie.ped, "DLC_24-1_YK_Survival_Sounds", 0, 0);
                break;
            case 0xAA4B67D0:
                AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "Deer_Pain", zombie.ped, "DLC_24-1_YK_Survival_Sounds", 0, 0);
                break;
            default:
                AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "Undead_Pain", zombie.ped, "DLC_24-1_YK_Survival_Sounds", 0, 0);
                break;
            }
        }
    }
    else
    {
        int currentTime = MISC::GET_GAME_TIMER();

        if (zombie.lastDialog - currentTime >= 2000 && CALC::RanInt(100, 0) <= 70)
        {
            zombie.lastDialog = currentTime;
            PlayZombieSound(zombie.ped, 8, 10);
        }
    }
}

int EnemyCountByType(eEnemyType type)
{
    int count = 0;

    for (Enemy& enemy : footEnemies)
    {
        if (enemy.enemyType == type)
        {
            count += 1;
        }
    }

    return count;
}

bool ShouldSpawnJuggernaut()
{
    if (SURVIVAL::SurvivalData::zombies)
    {
        bool timeLimit = false;
        int currentTime = MISC::GET_GAME_TIMER();
        int wave = SURVIVAL::SurvivalData::CurrentWave;
        int limit = 1;

        if (wave <= 4)
        {
            limit = 1;
        }
        else if (wave <= 6)
        {
            limit = 2;
        }
        else {
            limit = 3;
        }

        bool tankLimitReached = EnemyCountByType(eEnemyType::Juggernaut) >= limit;

        if (currentTime - lastTankCheck >= 4500)
        {
            timeLimit = true;
            lastTankCheck = currentTime;
        }

        return SURVIVAL::SurvivalData::CurrentWave >= 3 && !tankLimitReached && ENEMIES::EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize / 2 && timeLimit ;
    }

    bool tankLimitReached = EnemyCountByType(eEnemyType::Juggernaut) >= 1;

    return (SURVIVAL::SurvivalData::CurrentWave >= 8 || SURVIVAL::SurvivalData::hardcore) && SURVIVAL::SpawnerData::hasJuggernaut && !tankLimitReached && ENEMIES::EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize / 2;
}

bool ShouldSpawnAnimal()
{
    if (SURVIVAL::SurvivalData::zombies)
    {
        bool timeLimit = false;
        int currentTime = MISC::GET_GAME_TIMER();
        int wave = SURVIVAL::SurvivalData::CurrentWave;
        int limit = wave >= 7 ? 2 : 3;

        bool beastLimitReached = EnemyCountByType(eEnemyType::Animal) >= limit;


        if (currentTime - lastBeastCheck >= 4500)
        {
            timeLimit = true;
            lastBeastCheck = currentTime;
        }

        return SURVIVAL::SurvivalData::CurrentWave >= 4 && !beastLimitReached && timeLimit;
    }

    bool beastLimitReached = EnemyCountByType(eEnemyType::Animal) >= 3;

    return (SURVIVAL::SurvivalData::CurrentWave >= 7 || SURVIVAL::SurvivalData::hardcore) && SURVIVAL::SpawnerData::hasDogs && !beastLimitReached;
}

bool ShouldSpawnSuicidal()
{
    if (SURVIVAL::SurvivalData::zombies)
    {
        bool timeLimit = false;
        int currentTime = MISC::GET_GAME_TIMER();
        int wave = SURVIVAL::SurvivalData::CurrentWave;
        int limit = 1;

        if (wave <= 7)
        {
            limit = 2;
        }
        else
        {
            limit = 3;
        }

        bool explosiveLimitReached = EnemyCountByType(eEnemyType::Explosive) >= limit;

        if (currentTime - lastExplosiveCheck >= 3000)
        {
            timeLimit = true;
            lastExplosiveCheck = currentTime;
        }

        return SURVIVAL::SurvivalData::CurrentWave >= 6 && !explosiveLimitReached && timeLimit && CALC::RanInt(100, 1) <= 35;
    }

    bool explosiveLimitReached = EnemyCountByType(eEnemyType::Explosive) >= 2;

    return (SURVIVAL::SurvivalData::CurrentWave >= 5 || SURVIVAL::SurvivalData::hardcore) && SURVIVAL::SpawnerData::hasSuicidal && CALC::RanInt(100, 1) <= 20 && !explosiveLimitReached;
}

void ENEMIES::Process()
{
    if (!MUSIC::MusicData::finalWaveHI && SURVIVAL::SurvivalData::CurrentWave == 10 && !SURVIVAL::SurvivalData::InfiniteWaves && !SURVIVAL::SurvivalData::timed && EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize / 2)
    {
        MUSIC::FinalWaveMusicHighIntensity();
        MUSIC::MusicData::finalWaveHI = true;
    }

    if (EnemiesData::canSpawnMore && !EnemiesData::limitReached)
    {
        if (ShouldSpawnAnimal())
        {
            if (TIMERS::ProcessDogTimer())
            {
                Ped ped = SURVIVAL::SpawnDog();
                Enemy enemy = Enemy();
                enemy.ped = ped;
                enemy.enemyType = eEnemyType::Animal;
                InitializeAnimal(ped);
                footEnemies.push_back(enemy);

                TIMERS::RestartDogTimer();
            }
        }

        if (TIMERS::ProcessSpawnMoreTimer())
        {
            EnemiesData::currentWaveSize += 1;
            Ped ped;
            Enemy enemy = Enemy();

            if (ShouldSpawnJuggernaut())
            {
                ped = SURVIVAL::SpawnJuggernaut();
                enemy.ped = ped;
                enemy.enemyType = eEnemyType::Juggernaut;
                InitializeJuggernaut(enemy);
            }
            else if (ShouldSpawnSuicidal())
            {
                ped = SURVIVAL::SpawnEnemy(SURVIVAL::SurvivalData::CurrentWave, false, true);
                enemy.ped = ped;
                enemy.enemyType = eEnemyType::Explosive;
                InitializeSuicidal(ped);
            }
            else
            {
                ped = SURVIVAL::SpawnEnemy(SURVIVAL::SurvivalData::CurrentWave, !jesusSpawned);
                enemy.ped = ped;

                if (SURVIVAL::SurvivalData::zombies)
                {
                    enemy.enemyType = eEnemyType::Zombie;
                }

                InitializeEnemy(enemy);
            }

            footEnemies.push_back(enemy);
            TIMERS::RestartEnemyTimer();

            if (EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize && !SURVIVAL::SurvivalData::timed)
            {
                EnemiesData::canSpawnMore = false;
            }
        }

        if (SURVIVAL::SpawnerData::hasVehicles && (SURVIVAL::SurvivalData::CurrentWave >= 3 || SURVIVAL::SurvivalData::hardcore) && EnemiesData::currentVehicles < SURVIVAL::SurvivalData::MaxVehicles)
        {
            if (TIMERS::ProcessVehicleTimer())
            {
                bool boat = SURVIVAL::SpawnerData::hasBoats && CALC::RanInt(100, 1) <= 35;
                Vehicle vehicle = SURVIVAL::SpawnVehicle(boat);
                std::vector<Ped> peds = SURVIVAL::SpawnEnemiesInVehicle(vehicle, SURVIVAL::SurvivalData::CurrentWave);

                for (size_t i = 0; i < peds.size(); i++)
                {
                    Enemy enemy = Enemy();
                    enemy.ped = peds.at(i);
                    InitializeEnemyInVehicle(peds.at(i), i != 0);
                    footEnemies.push_back(enemy);
                }

                BLIPS::CreateForEnemyVehicle(vehicle);
                enemyVehicles.push_back(vehicle);
                EnemiesData::currentVehicles += 1;
                EnemiesData::currentWaveSize += peds.size();
                TIMERS::RestartVehicleTimer();

                if (EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize && !SURVIVAL::SurvivalData::timed)
                {
                    EnemiesData::canSpawnMore = false;
                }
            }
        }

        if (SURVIVAL::SpawnerData::hasAircraft && (SURVIVAL::SurvivalData::CurrentWave >= 5 || SURVIVAL::SurvivalData::hardcore) && EnemiesData::currentAircraft < SURVIVAL::SurvivalData::MaxAircraft)
        {
            if (TIMERS::ProcessAircraftTimer())
            {
                Vehicle aircraft = SURVIVAL::SpawnAircraft();
                std::vector<Ped> peds = SURVIVAL::SpawnEnemiesInVehicle(aircraft, SURVIVAL::SurvivalData::CurrentWave);

                for (size_t i = 0; i < peds.size(); i++)
                {
                    Enemy enemy = Enemy();
                    enemy.ped = peds.at(i);
                    InitializeEnemyInAircraft(peds.at(i), i != 0);
                    footEnemies.push_back(enemy);
                }

                BLIPS::CreateForEnemyVehicle(aircraft);
                enemyVehicles.push_back(aircraft);

                EnemiesData::currentAircraft += 1;
                EnemiesData::currentWaveSize += peds.size();
                TIMERS::RestartAircraftTimer();

                if (EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize && !SURVIVAL::SurvivalData::timed)
                {
                    EnemiesData::canSpawnMore = false;
                }
            }
        }
    }

    if (SURVIVAL::SurvivalData::timed)
    {
        if (TIMERS::ProcessTimedSurvivalTimer((SURVIVAL::SurvivalData::timedTimeLeft)))
        {
            SURVIVAL::QuitSurvival(true);
            return;
        }
    }

    if (!footEnemies.empty())
    {
        if (!TIMERS::LeavingZone::Started) {
            if (footEnemies.size() == 1 && !EnemiesData::canSpawnMore)
                SCREEN::ShowSubtitle("Kill the remaining ~r~enemy.", 8000);
            else
                SCREEN::ShowSubtitle("Kill the ~r~enemies.", 8000);
        }

        ProcessSuicidals();

        if (SURVIVAL::SurvivalData::zombies)
        {
            bool fieryZombieInRange = false;
            Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);

            for (Enemy& enemy : footEnemies)
            {
                if (!PED::IS_PED_DEAD_OR_DYING(enemy.ped, 1))
                {
                    PED::SET_PED_RESET_FLAG(enemy.ped, 440, true);
                    Vector3 zombieCoords = ENTITY::GET_ENTITY_COORDS(enemy.ped, true);

                    if (!fieryZombieInRange && enemy.fiery)
                    {
                        fieryZombieInRange = SYSTEM::VDIST2(playerCoords.x, playerCoords.y, playerCoords.z, zombieCoords.x, zombieCoords.y, zombieCoords.z) <= 4;
                    }

                    if (fieryZombieInRange && !playerOnFireByFiery)
                    {
                        playerOnFireByFiery = true;
                        FIRE::START_ENTITY_FIRE(PLAYER::PLAYER_PED_ID());
                    }
                }

                CheckDamageEvent(enemy);
            }

            if (playerOnFireByFiery && !fieryZombieInRange)
            {
                FIRE::STOP_ENTITY_FIRE(PLAYER::PLAYER_PED_ID());
            }
        }

        RemoveDeadEnemies();
        ProcessJesus();

        MUSIC::Process(EnemiesData::currentWaveSize, SURVIVAL::SurvivalData::MaxWaveSize);
        
        if (!SURVIVAL::SurvivalData::hardcore)
        {
            switch (SURVIVAL::SurvivalData::CurrentWave)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    EnemiesData::limitReached = footEnemies.size() >= 12;
                    break;
                case 6:
                case 7:
                    EnemiesData::limitReached = footEnemies.size() >= 11;
                    break;
                default:
                    EnemiesData::limitReached = footEnemies.size() >= 10;
                    break;
            }
        }

        return;
    }
    else if (!EnemiesData::canSpawnMore)
    {
        if (SURVIVAL::SurvivalData::InfiniteWaves || SURVIVAL::SurvivalData::CurrentWave < 10)
        {
            SCREEN::ShowSubtitle("", 1);
            INTERMISSION::Start();
        }
        else
        {
            SURVIVAL::CompleteSurvival();
        }
    }
}
