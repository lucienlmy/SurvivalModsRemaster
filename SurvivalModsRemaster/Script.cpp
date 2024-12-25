#include "pch.h"
#include "Script.hpp"
#include "json.hpp"

#include <array>

std::vector<SurvivalAllies> TriggerPedsData::allies;
std::vector<std::string> TriggerPedsData::names;
std::vector<std::string> TriggerPedsData::models;
std::vector<std::string> TriggerPedsData::tasks;
std::vector<Ped> TriggerPedsData::peds;
std::vector<EntityPosition> TriggerPedsData::positions;
std::vector<Blip> TriggerPedsData::blips;
std::vector<bool> TriggerPedsData::timerActive;
std::vector<bool> TriggerPedsData::killedFlags;
std::vector<int> TriggerPedsData::starTime;
std::vector<int> TriggerPedsData::playerRel;
int Data::intermissionDuration;
Controls Data::infiniteWaveControl;
Controls Data::timedSurvivalControl;
Controls Data::cancelControl;
Controls Data::reloadTriggerPedsControl;
Controls Data::hardcoreSurvivalControl;
bool Data::showControls;
bool canStart;
int cancelStartTime;
int cancelCurrentTime;
Hash Data::enemiesRelGroup;
Hash Data::neutralRelGroup;
nlohmann::json j;
int Data::TPIndex;
bool tpPointsEnabled;
int playerId;
SurvivalModes currentMode = SurvivalModes::TenWaves;
bool islandLoaded = false;

std::array<const char*, 319> CAYO_PERICO_IPL = {
    "h4_mph4_terrain_01_grass_0",
    "h4_mph4_terrain_01_grass_1",
    "h4_mph4_terrain_02_grass_0",
    "h4_mph4_terrain_02_grass_1",
    "h4_mph4_terrain_02_grass_2",
    "h4_mph4_terrain_02_grass_3",
    "h4_mph4_terrain_04_grass_0",
    "h4_mph4_terrain_04_grass_1",
    "h4_mph4_terrain_05_grass_0",
    "h4_mph4_terrain_06_grass_0",
    "h4_islandx_terrain_01",
    "h4_islandx_terrain_01_lod",
    "h4_islandx_terrain_01_slod",
    "h4_islandx_terrain_02",
    "h4_islandx_terrain_02_lod",
    "h4_islandx_terrain_02_slod",
    "h4_islandx_terrain_03",
    "h4_islandx_terrain_03_lod",
    "h4_islandx_terrain_04",
    "h4_islandx_terrain_04_lod",
    "h4_islandx_terrain_04_slod",
    "h4_islandx_terrain_05",
    "h4_islandx_terrain_05_lod",
    "h4_islandx_terrain_05_slod",
    "h4_islandx_terrain_06",
    "h4_islandx_terrain_06_lod",
    "h4_islandx_terrain_06_slod",
    "h4_islandx_terrain_props_05_a",
    "h4_islandx_terrain_props_05_a_lod",
    "h4_islandx_terrain_props_05_b",
    "h4_islandx_terrain_props_05_b_lod",
    "h4_islandx_terrain_props_05_c",
    "h4_islandx_terrain_props_05_c_lod",
    "h4_islandx_terrain_props_05_d",
    "h4_islandx_terrain_props_05_d_lod",
    "h4_islandx_terrain_props_05_d_slod",
    "h4_islandx_terrain_props_05_e",
    "h4_islandx_terrain_props_05_e_lod",
    "h4_islandx_terrain_props_05_e_slod",
    "h4_islandx_terrain_props_05_f",
    "h4_islandx_terrain_props_05_f_lod",
    "h4_islandx_terrain_props_05_f_slod",
    "h4_islandx_terrain_props_06_a",
    "h4_islandx_terrain_props_06_a_lod",
    "h4_islandx_terrain_props_06_a_slod",
    "h4_islandx_terrain_props_06_b",
    "h4_islandx_terrain_props_06_b_lod",
    "h4_islandx_terrain_props_06_b_slod",
    "h4_islandx_terrain_props_06_c",
    "h4_islandx_terrain_props_06_c_lod",
    "h4_islandx_terrain_props_06_c_slod",
    "h4_mph4_terrain_01",
    "h4_mph4_terrain_01_long_0",
    "h4_mph4_terrain_02",
    "h4_mph4_terrain_03",
    "h4_mph4_terrain_04",
    "h4_mph4_terrain_05",
    "h4_mph4_terrain_06",
    "h4_mph4_terrain_06_strm_0",
    "h4_mph4_terrain_lod",
    "h4_mph4_terrain_occ_00",
    "h4_mph4_terrain_occ_01",
    "h4_mph4_terrain_occ_02",
    "h4_mph4_terrain_occ_03",
    "h4_mph4_terrain_occ_04",
    "h4_mph4_terrain_occ_05",
    "h4_mph4_terrain_occ_06",
    "h4_mph4_terrain_occ_07",
    "h4_mph4_terrain_occ_08",
    "h4_mph4_terrain_occ_09",
    "h4_boatblockers",
    "h4_islandx",
    "h4_islandx_disc_strandedshark",
    "h4_islandx_disc_strandedshark_lod",
    "h4_islandx_disc_strandedwhale",
    "h4_islandx_disc_strandedwhale_lod",
    "h4_islandx_props",
    "h4_islandx_props_lod",
    "h4_islandx_sea_mines",
    "h4_mph4_island",
    "h4_mph4_island_long_0",
    "h4_mph4_island_strm_0",
    "h4_aa_guns",
    "h4_aa_guns_lod",
    "h4_beach",
    "h4_beach_bar_props",
    "h4_beach_lod",
    "h4_beach_party",
    "h4_beach_party_lod",
    "h4_beach_props",
    "h4_beach_props_lod",
    "h4_beach_props_party",
    "h4_beach_props_slod",
    "h4_beach_slod",
    "h4_islandairstrip",
    "h4_islandairstrip_doorsclosed",
    "h4_islandairstrip_doorsclosed_lod",
    "h4_islandairstrip_doorsopen",
    "h4_islandairstrip_doorsopen_lod",
    "h4_islandairstrip_hangar_props",
    "h4_islandairstrip_hangar_props_lod",
    "h4_islandairstrip_hangar_props_slod",
    "h4_islandairstrip_lod",
    "h4_islandairstrip_props",
    "h4_islandairstrip_propsb",
    "h4_islandairstrip_propsb_lod",
    "h4_islandairstrip_propsb_slod",
    "h4_islandairstrip_props_lod",
    "h4_islandairstrip_props_slod",
    "h4_islandairstrip_slod",
    "h4_islandxcanal_props",
    "h4_islandxcanal_props_lod",
    "h4_islandxcanal_props_slod",
    "h4_islandxdock",
    "h4_islandxdock_lod",
    "h4_islandxdock_props",
    "h4_islandxdock_props_2",
    "h4_islandxdock_props_2_lod",
    "h4_islandxdock_props_2_slod",
    "h4_islandxdock_props_lod",
    "h4_islandxdock_props_slod",
    "h4_islandxdock_slod",
    "h4_islandxdock_water_hatch",
    "h4_islandxtower",
    "h4_islandxtower_lod",
    "h4_islandxtower_slod",
    "h4_islandxtower_veg",
    "h4_islandxtower_veg_lod",
    "h4_islandxtower_veg_slod",
    "h4_islandx_barrack_hatch",
    "h4_islandx_barrack_props",
    "h4_islandx_barrack_props_lod",
    "h4_islandx_barrack_props_slod",
    "h4_islandx_checkpoint",
    "h4_islandx_checkpoint_lod",
    "h4_islandx_checkpoint_props",
    "h4_islandx_checkpoint_props_lod",
    "h4_islandx_checkpoint_props_slod",
    "h4_islandx_maindock",
    "h4_islandx_maindock_lod",
    "h4_islandx_maindock_props",
    "h4_islandx_maindock_props_2",
    "h4_islandx_maindock_props_2_lod",
    "h4_islandx_maindock_props_2_slod",
    "h4_islandx_maindock_props_lod",
    "h4_islandx_maindock_props_slod",
    "h4_islandx_maindock_slod",
    "h4_islandx_mansion",
    "h4_islandx_mansion_b",
    "h4_islandx_mansion_b_lod",
    "h4_islandx_mansion_b_side_fence",
    "h4_islandx_mansion_b_slod",
    "h4_islandx_mansion_entrance_fence",
    "h4_islandx_mansion_guardfence",
    "h4_islandx_mansion_lights",
    "h4_islandx_mansion_lockup_01",
    "h4_islandx_mansion_lockup_01_lod",
    "h4_islandx_mansion_lockup_02",
    "h4_islandx_mansion_lockup_02_lod",
    "h4_islandx_mansion_lockup_03",
    "h4_islandx_mansion_lockup_03_lod",
    "h4_islandx_mansion_lod",
    "h4_islandx_mansion_office",
    "h4_islandx_mansion_office_lod",
    "h4_islandx_mansion_props",
    "h4_islandx_mansion_props_lod",
    "h4_islandx_mansion_props_slod",
    "h4_islandx_mansion_slod",
    "h4_islandx_mansion_vault",
    "h4_islandx_mansion_vault_lod",
    "h4_island_padlock_props",
    "h4_mansion_gate_broken",
    "h4_mansion_remains_cage",
    "h4_mph4_airstrip",
    "h4_mph4_airstrip_interior_0_airstrip_hanger",
    "h4_mph4_beach",
    "h4_mph4_dock",
    "h4_mph4_island_lod",
    "h4_mph4_island_ne_placement",
    "h4_mph4_island_nw_placement",
    "h4_mph4_island_se_placement",
    "h4_mph4_island_sw_placement",
    "h4_mph4_mansion",
    "h4_mph4_mansion_b",
    "h4_mph4_mansion_b_strm_0",
    "h4_mph4_mansion_strm_0",
    "h4_mph4_wtowers",
    "h4_ne_ipl_00",
    "h4_ne_ipl_00_lod",
    "h4_ne_ipl_00_slod",
    "h4_ne_ipl_01",
    "h4_ne_ipl_01_lod",
    "h4_ne_ipl_01_slod",
    "h4_ne_ipl_02",
    "h4_ne_ipl_02_lod",
    "h4_ne_ipl_02_slod",
    "h4_ne_ipl_03",
    "h4_ne_ipl_03_lod",
    "h4_ne_ipl_03_slod",
    "h4_ne_ipl_04",
    "h4_ne_ipl_04_lod",
    "h4_ne_ipl_04_slod",
    "h4_ne_ipl_05",
    "h4_ne_ipl_05_lod",
    "h4_ne_ipl_05_slod",
    "h4_ne_ipl_06",
    "h4_ne_ipl_06_lod",
    "h4_ne_ipl_06_slod",
    "h4_ne_ipl_07",
    "h4_ne_ipl_07_lod",
    "h4_ne_ipl_07_slod",
    "h4_ne_ipl_08",
    "h4_ne_ipl_08_lod",
    "h4_ne_ipl_08_slod",
    "h4_ne_ipl_09",
    "h4_ne_ipl_09_lod",
    "h4_ne_ipl_09_slod",
    "h4_nw_ipl_00",
    "h4_nw_ipl_00_lod",
    "h4_nw_ipl_00_slod",
    "h4_nw_ipl_01",
    "h4_nw_ipl_01_lod",
    "h4_nw_ipl_01_slod",
    "h4_nw_ipl_02",
    "h4_nw_ipl_02_lod",
    "h4_nw_ipl_02_slod",
    "h4_nw_ipl_03",
    "h4_nw_ipl_03_lod",
    "h4_nw_ipl_03_slod",
    "h4_nw_ipl_04",
    "h4_nw_ipl_04_lod",
    "h4_nw_ipl_04_slod",
    "h4_nw_ipl_05",
    "h4_nw_ipl_05_lod",
    "h4_nw_ipl_05_slod",
    "h4_nw_ipl_06",
    "h4_nw_ipl_06_lod",
    "h4_nw_ipl_06_slod",
    "h4_nw_ipl_07",
    "h4_nw_ipl_07_lod",
    "h4_nw_ipl_07_slod",
    "h4_nw_ipl_08",
    "h4_nw_ipl_08_lod",
    "h4_nw_ipl_08_slod",
    "h4_nw_ipl_09",
    "h4_nw_ipl_09_lod",
    "h4_nw_ipl_09_slod",
    "h4_se_ipl_00",
    "h4_se_ipl_00_lod",
    "h4_se_ipl_00_slod",
    "h4_se_ipl_01",
    "h4_se_ipl_01_lod",
    "h4_se_ipl_01_slod",
    "h4_se_ipl_02",
    "h4_se_ipl_02_lod",
    "h4_se_ipl_02_slod",
    "h4_se_ipl_03",
    "h4_se_ipl_03_lod",
    "h4_se_ipl_03_slod",
    "h4_se_ipl_04",
    "h4_se_ipl_04_lod",
    "h4_se_ipl_04_slod",
    "h4_se_ipl_05",
    "h4_se_ipl_05_lod",
    "h4_se_ipl_05_slod",
    "h4_se_ipl_06",
    "h4_se_ipl_06_lod",
    "h4_se_ipl_06_slod",
    "h4_se_ipl_07",
    "h4_se_ipl_07_lod",
    "h4_se_ipl_07_slod",
    "h4_se_ipl_08",
    "h4_se_ipl_08_lod",
    "h4_se_ipl_08_slod",
    "h4_se_ipl_09",
    "h4_se_ipl_09_lod",
    "h4_se_ipl_09_slod",
    "h4_sw_ipl_00",
    "h4_sw_ipl_00_lod",
    "h4_sw_ipl_00_slod",
    "h4_sw_ipl_01",
    "h4_sw_ipl_01_lod",
    "h4_sw_ipl_01_slod",
    "h4_sw_ipl_02",
    "h4_sw_ipl_02_lod",
    "h4_sw_ipl_02_slod",
    "h4_sw_ipl_03",
    "h4_sw_ipl_03_lod",
    "h4_sw_ipl_03_slod",
    "h4_sw_ipl_04",
    "h4_sw_ipl_04_lod",
    "h4_sw_ipl_04_slod",
    "h4_sw_ipl_05",
    "h4_sw_ipl_05_lod",
    "h4_sw_ipl_05_slod",
    "h4_sw_ipl_06",
    "h4_sw_ipl_06_lod",
    "h4_sw_ipl_06_slod",
    "h4_sw_ipl_07",
    "h4_sw_ipl_07_lod",
    "h4_sw_ipl_07_slod",
    "h4_sw_ipl_08",
    "h4_sw_ipl_08_lod",
    "h4_sw_ipl_08_slod",
    "h4_sw_ipl_09",
    "h4_sw_ipl_09_lod",
    "h4_sw_ipl_09_slod",
    "h4_underwater_gate_closed",
    "h4_islandx_placement_01",
    "h4_islandx_placement_02",
    "h4_islandx_placement_03",
    "h4_islandx_placement_04",
    "h4_islandx_placement_05",
    "h4_islandx_placement_06",
    "h4_islandx_placement_07",
    "h4_islandx_placement_08",
    "h4_islandx_placement_09",
    "h4_islandx_placement_10",
    "h4_mph4_island_placement"
};

Vector3 CayoPericoCoords = Vector3(5031.428f, -5150.907f, 0);

struct TPPoint {
    float x;
    float y;
    float z;

    TPPoint(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

enum eMarkers {
    BunkerEntrance,
    BunkerExit,
    LabEntrance,
    LabExit,
    CemeteryEntrance,
    CemeteryExit,
    CayoPericoEntrance,
    CayoPericoExit
};

static std::vector<TPPoint> teleportPoints = std::vector<TPPoint>{
        TPPoint(1571.56f, 2225.47f, 77.32f),
        TPPoint(893.14f, -3245.89f, -99.25251f),
        TPPoint(456.766663f, 5571.864f, 780.1841f),
        TPPoint(244.57f, 6163.39f, -160.42f),
        TPPoint(-1729.10, -193.10, 58.52),
        TPPoint(3224.75, -4702.88, 112.74),
        TPPoint(-932.5, -1474.9, 1),
        TPPoint(4888.059, -5175.88, 2.61)
};

static std::vector<Blip> entranceBlips = std::vector<Blip>();

void SetAllies() {
    size_t size = TriggerPedsData::allies.size();
    std::string name = TriggerPedsData::names.at(Data::TPIndex);

    for (size_t i = 0; i < size; i++) {
        SurvivalAllies ally = TriggerPedsData::allies.at(i);

        if (ally.MissionID == name) {
            PED::SET_RELATIONSHIP_BETWEEN_GROUPS(1, Data::neutralRelGroup,
                                                 MISC::GET_HASH_KEY(ally.RelGroupName.c_str()));
            PED::SET_RELATIONSHIP_BETWEEN_GROUPS(1, MISC::GET_HASH_KEY(ally.RelGroupName.c_str()),
                                                 Data::neutralRelGroup);
        }
    }
}

void ClearAllies(bool all = false) {
    size_t size = TriggerPedsData::allies.size();
    std::string name = TriggerPedsData::names.at(Data::TPIndex);

    for (size_t i = 0; i < size; i++) {
        SurvivalAllies ally = TriggerPedsData::allies.at(i);

        if (ally.MissionID == name || all) {
            PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, Data::neutralRelGroup,
                                                 MISC::GET_HASH_KEY(ally.RelGroupName.c_str()));
            PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, MISC::GET_HASH_KEY(ally.RelGroupName.c_str()),
                                                 Data::neutralRelGroup);
        }
    }
}

void IsPlayerInMissionStartRange() {
    if (SURVIVAL::SurvivalData::IsActive || PLAYER::GET_PLAYER_WANTED_LEVEL(playerId) > 1
        || MISC::GET_MISSION_FLAG()) {
        Data::showControls = false;
        canStart = false;
        return;
    }

    Vector3 playerPosition = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
    size_t size = TriggerPedsData::peds.size();

    for (int i = 0; i < size; i++) {
        if (CALC::IsInRange_2(playerPosition, TriggerPedsData::positions.at(i).coords, 80.0f)) {
            Data::TPIndex = i;
            canStart = true;

            Ped ped = TriggerPedsData::peds.at(i);

            if (ped == 0) {
                Data::showControls = false;
                return;
            }

            Vector3 coords = ENTITY::GET_ENTITY_COORDS(ped, false);
            Data::showControls = CALC::IsInRange_2(playerPosition, coords, 4.5f) &&
                                 !PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false);
            return;
        }
    }

    Data::showControls = false;
    canStart = false;
}

void ReadConfig() {
    std::ifstream i("SurvivalsData\\config.json");
    i >> j;
    i.close();

    Data::intermissionDuration = j["Gameplay"]["IntermissionDuration"];
    tpPointsEnabled = j["Gameplay"]["TPMarkers"];
    Data::infiniteWaveControl = static_cast<Controls>(j["Controls"]["StartInfiniteWaves"]);
    Data::timedSurvivalControl = static_cast<Controls>(j["Controls"]["StartTimedSurvival"]);
    Data::cancelControl = static_cast<Controls>(j["Controls"]["CancelSurvival"]);
    Data::reloadTriggerPedsControl = static_cast<Controls>(j["Controls"]["ReloadTriggerPeds"]);
    Data::hardcoreSurvivalControl = static_cast<Controls>(j["Controls"]["StartHardcoreSurvival"]);

    Hash playerGroupHash = MISC::GET_HASH_KEY("PLAYER");
    Hash cougarGroupHash = MISC::GET_HASH_KEY("COUGAR");
    PED::ADD_RELATIONSHIP_GROUP("SURVIVAL_MISSION_ENEMIES_REL_GROUP", &Data::enemiesRelGroup);
    PED::ADD_RELATIONSHIP_GROUP("SURVIVAL_MISSION_TRIGGER_REL_GROUP", &Data::neutralRelGroup);
    PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, Data::enemiesRelGroup, playerGroupHash);
    PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, Data::enemiesRelGroup);
    PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, Data::enemiesRelGroup, cougarGroupHash);
    PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, cougarGroupHash, Data::enemiesRelGroup);
    PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, Data::neutralRelGroup, playerGroupHash);
    PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, playerGroupHash, Data::neutralRelGroup);

    INIT::LoadTriggerPeds();
}

void createTPBlips() {
    STREAMING::REQUEST_IPL("gr_case6_bunkerclosed");
    STREAMING::REQUEST_IPL("gr_grdlc_interior_placement_interior_0_grdlc_int_01_milo_");
    STREAMING::REQUEST_IPL("gr_grdlc_interior_placement_interior_1_grdlc_int_02_milo_");
    TPPoint coords = teleportPoints.at(eMarkers::BunkerEntrance);
    entranceBlips.push_back(BLIPS::Create(coords.x, coords.y, coords.z, 557, eBlipColor::BlipColorWhite, "Bunker"));
    coords = teleportPoints.at(eMarkers::LabEntrance);
    entranceBlips.push_back(BLIPS::Create(coords.x, coords.y, coords.z, 499, eBlipColor::BlipColorWhite, "Secret Lab"));
    coords = teleportPoints.at(eMarkers::CemeteryEntrance);
    entranceBlips.push_back(BLIPS::Create(coords.x, coords.y, coords.z, 465, eBlipColor::BlipColorWhite, "Cemetery"));
}

const char* getHelpText(size_t index) {
    switch (index) {
        case eMarkers::BunkerEntrance:
            return "Press ~INPUT_CONTEXT~ to enter the bunker.";
        case eMarkers::BunkerExit:
            return "Press ~INPUT_CONTEXT~ to exit the bunker.";
        case eMarkers::LabEntrance:
            return "Press ~INPUT_CONTEXT~ to enter the lab.";
        case eMarkers::LabExit:
            return "Press ~INPUT_CONTEXT~ to exit the lab.";
        case eMarkers::CemeteryEntrance:
            return "Press ~INPUT_CONTEXT~ to investigate.";
        case eMarkers::CemeteryExit:
            return "Press ~INPUT_CONTEXT~ to leave.";
        default:
            return "INVALID INDEX";
    }
}

void LoadNY()
{
    STREAMING::REQUEST_IPL("prologue01k");
    STREAMING::REQUEST_IPL("prologue02");
    STREAMING::REQUEST_IPL("prologue03");
    STREAMING::REQUEST_IPL("prologue03b");
    STREAMING::REQUEST_IPL("prologue03_grv_dug");
    STREAMING::REQUEST_IPL("prologue04");
    STREAMING::REQUEST_IPL("prologue04b");
    STREAMING::REQUEST_IPL("prologue05");
    STREAMING::REQUEST_IPL("prologue05b");
    STREAMING::REQUEST_IPL("prologue_occl");
    STREAMING::REQUEST_IPL("prologue_LODLights");
    STREAMING::REQUEST_IPL("prologue_DistantLights");

    while (
        !STREAMING::IS_IPL_ACTIVE("prologue01k") ||
        !STREAMING::IS_IPL_ACTIVE("prologue02") ||
        !STREAMING::IS_IPL_ACTIVE("prologue03") ||
        !STREAMING::IS_IPL_ACTIVE("prologue03b") ||
        !STREAMING::IS_IPL_ACTIVE("prologue03_grv_dug") ||
        !STREAMING::IS_IPL_ACTIVE("prologue04") ||
        !STREAMING::IS_IPL_ACTIVE("prologue04b") ||
        !STREAMING::IS_IPL_ACTIVE("prologue05") ||
        !STREAMING::IS_IPL_ACTIVE("prologue05b") ||
        !STREAMING::IS_IPL_ACTIVE("prologue_occl") ||
        !STREAMING::IS_IPL_ACTIVE("prologue_LODLights") ||
        !STREAMING::IS_IPL_ACTIVE("prologue_DistantLights")
        ) {
        WAIT(50);
    }

    HUD::SET_MINIMAP_IN_PROLOGUE(true);
    ZONE::SET_ZONE_ENABLED(ZONE::GET_ZONE_FROM_NAME_ID("PrLog"), true);
    STREAMING::SET_MAPDATACULLBOX_ENABLED("prologue", true);
}

void UnloadNY()
{
    STREAMING::REMOVE_IPL("prologue01k");
    STREAMING::REMOVE_IPL("prologue02");
    STREAMING::REMOVE_IPL("prologue03");
    STREAMING::REMOVE_IPL("prologue03b");
    STREAMING::REMOVE_IPL("prologue03_grv_dug");
    STREAMING::REMOVE_IPL("prologue04");
    STREAMING::REMOVE_IPL("prologue04b");
    STREAMING::REMOVE_IPL("prologue05");
    STREAMING::REMOVE_IPL("prologue05b");
    STREAMING::REMOVE_IPL("prologue_occl");
    STREAMING::REMOVE_IPL("prologue_LODLights");
    STREAMING::REMOVE_IPL("prologue_DistantLights");

    HUD::SET_MINIMAP_IN_PROLOGUE(false);
    ZONE::SET_ZONE_ENABLED(ZONE::GET_ZONE_FROM_NAME_ID("PrLog"), false);
    STREAMING::SET_MAPDATACULLBOX_ENABLED("prologue", false);
}

void LoadBunker()
{
    TPPoint v = teleportPoints.at(eMarkers::BunkerExit);
    Interior i = INTERIOR::GET_INTERIOR_AT_COORDS(v.x, v.y, v.z);

    if (!INTERIOR::IS_VALID_INTERIOR(i))
    {
        SCREEN::ShowNotification("INVALID BUNKER INTERIOR");
        return;
    }

    if (INTERIOR::IS_INTERIOR_DISABLED(i))
    {
        INTERIOR::DISABLE_INTERIOR(i, false);
    }

    if (INTERIOR::IS_INTERIOR_CAPPED(i))
    {
        INTERIOR::CAP_INTERIOR(i, false);
    }

    INTERIOR::PIN_INTERIOR_IN_MEMORY(i);

    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "bunker_style_a");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "bunker_style_b");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "standard_bunker_set");

    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "bunker_style_c");
    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "upgrade_bunker_set");
    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "upgrade_bunker_set_more");
    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "security_upgrade");
    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "Office_Upgrade_set");
    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "gun_range_lights");
    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "gun_schematic_set");
    INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(i, "gun_locker_upgrade");

    INTERIOR::REFRESH_INTERIOR(i);

    WAIT(1200);
}

void UnloadBunker()
{
    TPPoint v = teleportPoints.at(eMarkers::BunkerExit);
    Interior i = INTERIOR::GET_INTERIOR_AT_COORDS(v.x, v.y, v.z);

    if (!INTERIOR::IS_VALID_INTERIOR(i))
    {
        SCREEN::ShowNotification("INVALID BUNKER INTERIOR");
        return;
    }

    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "bunker_style_c");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "upgrade_bunker_set");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "upgrade_bunker_set_more");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "security_upgrade");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "Office_Upgrade_set");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "gun_range_lights");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "gun_schematic_set");
    INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(i, "gun_locker_upgrade");

    if (!INTERIOR::IS_INTERIOR_DISABLED(i))
    {
        INTERIOR::DISABLE_INTERIOR(i, true);
    }

    if (!INTERIOR::IS_INTERIOR_CAPPED(i))
    {
        INTERIOR::CAP_INTERIOR(i, true);
    }

    INTERIOR::UNPIN_INTERIOR(i);
}

void LoadCayoPerico()
{
    for (const char*& ipl : CAYO_PERICO_IPL)
    {
        STREAMING::REQUEST_IPL(ipl);
    }

    bool cayoPericoLoaded = false;

    do {
        cayoPericoLoaded = true;

        for (const char*& ipl : CAYO_PERICO_IPL)
        {
            if (!STREAMING::IS_IPL_ACTIVE(ipl)) {
                cayoPericoLoaded = false;
            }
        }
    } while (!cayoPericoLoaded);
}

void UnloadCayoPerico()
{
    for (const char*& ipl : CAYO_PERICO_IPL)
    {
        STREAMING::REMOVE_IPL(ipl);
    }
}

void toggleInterior(size_t index) {
    switch (index) {
        case eMarkers::BunkerEntrance: {
            AUDIO::PLAY_SOUND_FRONTEND(-1, "Door_Open_Limit", "DLC_GR_Bunker_Door_Sounds", true);
            LoadBunker();
            break;
        }
        case eMarkers::BunkerExit: {
            AUDIO::PLAY_SOUND_FRONTEND(-1, "Enter_Car_Ramp_Deploy", "DLC_GR_MOC_Enter_Exit_Sounds", true);
            UnloadBunker();
            break;
        }
        case eMarkers::LabEntrance: {
            TPPoint v = teleportPoints.at(eMarkers::LabExit);
            Interior i = INTERIOR::GET_INTERIOR_AT_COORDS(v.x, v.y, v.z);
            INTERIOR::PIN_INTERIOR_IN_MEMORY(i);
            AUDIO::PLAY_SOUND_FRONTEND(-1, "Door_Open_Limit", "DLC_XM_Silo_Secret_Door_Sounds", true);
            break;
        }
        case eMarkers::LabExit: {
            TPPoint v = teleportPoints.at(eMarkers::LabExit);
            Interior i = INTERIOR::GET_INTERIOR_AT_COORDS(v.x, v.y, v.z);
            INTERIOR::UNPIN_INTERIOR(i);
            AUDIO::PLAY_SOUND_FRONTEND(-1, "DOOR_CLOSE", "CABLE_CAR_SOUNDS", true);
            break;
        }
        case eMarkers::CemeteryEntrance:
        {
            LoadNY();
            break;
        }
        case eMarkers::CemeteryExit:
        {
            UnloadNY();
            break;
        }
        case eMarkers::CayoPericoEntrance:
        {
            LoadCayoPerico();
            break;
        }
        case eMarkers::CayoPericoExit:
        {
            UnloadCayoPerico();
            break;
        }
        default:
            break;
    }
}

void processMarkers() {
    size_t size = teleportPoints.size();
    
    for (size_t i = 0; i < size; i++) {
        TPPoint coords = teleportPoints.at(i);
        Vector3 v3Coords = Vector3();
        v3Coords.x = coords.x;
        v3Coords.y = coords.y;
        v3Coords.z = coords.z;
        Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(playerId, true);

        if (!CALC::IsInRange_2(v3Coords, playerCoords, 5))
        {
            continue;
        }

        SCREEN::ShowHelpText(getHelpText(i), true);

        if (PAD::IS_CONTROL_JUST_PRESSED(0, 51)) {
            CAM::DO_SCREEN_FADE_OUT(1000);
            WAIT(1200);
            toggleInterior(i);
            size_t index;

            if (i % 2 == 0)
                index = i + 1;
            else
                index = i - 1;

            coords = teleportPoints.at(index);
            coords.z = coords.z + (i == 0 ? 1.0f : 0.0f);
            ENTITY::SET_ENTITY_COORDS(playerId, coords.x, coords.y, coords.z, 1, 0, 0, 1);
            WAIT(2000);
            CAM::DO_SCREEN_FADE_IN(1000);
        }
    }
}

void ControlsWatch() {
    if (!SURVIVAL::SurvivalData::IsActive && !canStart) {
        if (PAD::IS_CONTROL_JUST_PRESSED(0, static_cast<int>(Data::reloadTriggerPedsControl))) {
            INIT::LoadTriggerPeds();
        }
    }

    if (Data::showControls) {
        if (PAD::IS_CONTROL_JUST_PRESSED(0, static_cast<int>(Data::infiniteWaveControl))) {
            currentMode = SurvivalModes::Endless;
            SCREEN::ShowNotification("Survival mode set to: Endless survival.");
        }

        if (PAD::IS_CONTROL_JUST_PRESSED(0, static_cast<int>(Data::timedSurvivalControl))) {
            currentMode = SurvivalModes::Timed;
            SCREEN::ShowNotification("Survival mode set to: Timed survival.");
        }

        if (PAD::IS_CONTROL_JUST_PRESSED(0, static_cast<int>(Data::hardcoreSurvivalControl))) {
            currentMode = SurvivalModes::Hardcore;
            SCREEN::ShowNotification("Survival mode set to: Hardcore survival.");
        }
    } else {
        if (SURVIVAL::SurvivalData::IsActive) {
            if (PAD::IS_CONTROL_JUST_PRESSED(0, static_cast<int>(Data::cancelControl))) {
                cancelStartTime = MISC::GET_GAME_TIMER();
            }

            if (PAD::IS_CONTROL_PRESSED(0, static_cast<int>(Data::cancelControl))) {
                cancelCurrentTime = MISC::GET_GAME_TIMER();
                if (cancelCurrentTime - cancelStartTime >= 3000) {
                    SURVIVAL::QuitSurvival(false);
                }
            }
        }
        else {
            if (currentMode != SurvivalModes::TenWaves) {
                currentMode = SurvivalModes::TenWaves;
                SCREEN::ShowNotification("Survival mode set to: Ten Waves");
            }
        }
    }
}

bool KilledByPlayer(Ped ped) {
    int player = PLAYER::PLAYER_PED_ID();
    Entity killer = PED::GET_PED_SOURCE_OF_DEATH(ped);

    if (killer == player || killer == 0)
        return true;

    return false;
}

void ProcessTriggerPeds() {
    size_t size = TriggerPedsData::timerActive.size();

    for (size_t i = 0; i < size; i++) {
        bool active = TriggerPedsData::timerActive.at(i);

        if (!active) {
            if (!SURVIVAL::SurvivalData::IsActive) {
                Vector3 playerPosition = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
                EntityPosition TPPos = TriggerPedsData::positions.at(i);
                bool inRange = CALC::IsInRange_2(playerPosition, TPPos.coords, 80.0f);
                Ped ped = TriggerPedsData::peds.at(i);

                if (ped == 0 && canStart) {
                    if (inRange && !TriggerPedsData::killedFlags.at(i)) {
                        TriggerPedsData::peds.at(i) = INIT::SpawnTriggerPed(i);
                        SetAllies();
                    } else if (!inRange)
                        TriggerPedsData::killedFlags.at(i) = false;
                } else if (ped != 0) {
                    if (!inRange) {
                        ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
                        TriggerPedsData::peds.at(i) = 0;
                        ClearAllies();
                    } else if (!canStart) {
                        ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
                        TriggerPedsData::peds.at(i) = 0;
                        TriggerPedsData::killedFlags.at(i) = true;
                        ClearAllies();
                    } else if (ENTITY::IS_ENTITY_DEAD(ped, true)) {
                        if (KilledByPlayer(ped)) {
                            HUD::REMOVE_BLIP(&TriggerPedsData::blips.at(i));
                            TriggerPedsData::blips.at(i) = 0;
                            ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
                            TriggerPedsData::peds.at(i) = 0;
                            SURVIVAL::StartMission(
                                    currentMode == SurvivalModes::Endless,
                                    currentMode == SurvivalModes::Timed,
                                    currentMode == SurvivalModes::Hardcore
                                    );
                            currentMode = SurvivalModes::TenWaves;
                        } else {
                            ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
                            TriggerPedsData::peds.at(i) = 0;
                            TriggerPedsData::killedFlags.at(i) = true;
                        }

                        ClearAllies();
                    }
                }
            }

            continue;
        }

        if (MISC::GET_GAME_TIMER() - TriggerPedsData::starTime.at(i) < 300000) {
            continue;
        }

        TriggerPedsData::timerActive.at(i) = false;
        TriggerPedsData::blips.at(i) = BLIPS::CreateForMissionTriggerPed(TriggerPedsData::positions.at(i).coords,
                                                                         TriggerPedsData::names.at(i).c_str());
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    while (DLC::GET_IS_LOADING_SCREEN_ACTIVE()) {
        WAIT(0);
    }

    if (SURVIVAL::SurvivalData::IsActive) {
        OnAbort();
    }

    ReadConfig();

    if (tpPointsEnabled)
    {
        createTPBlips();
    }

    DLC::ON_ENTER_MP();
    MISC::SET_INSTANCE_PRIORITY_MODE(1);

    //Load bunker hatch for the bunker survival
    if (!STREAMING::IS_IPL_ACTIVE("gr_case6_bunkerclosed"))
    {
        STREAMING::REQUEST_IPL("gr_case6_bunkerclosed");
    }

    while (true) {
        playerId = PLAYER::PLAYER_PED_ID();
        Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(playerId, true);

        //load cayo perico when the player is in range
        if (MISC::GET_DISTANCE_BETWEEN_COORDS(playerCoords.x, playerCoords.y, 0, CayoPericoCoords.x, CayoPericoCoords.y, 0, false) < 2000)
        {
            if (!islandLoaded) {
                islandLoaded = true;
                WATER::SET_DEEP_OCEAN_SCALER(0);
                TASK::SET_SCENARIO_GROUP_ENABLED("Heist_Island_Peds", true);
                STREAMING::SET_ISLAND_ENABLED("HeistIsland", true);
                PATHFIND::SET_ALLOW_STREAM_HEIST_ISLAND_NODES(1);
                AUDIO::SET_AMBIENT_ZONE_LIST_STATE_PERSISTENT("AZL_DLC_Hei4_Island_Zones", true, true);
                AUDIO::SET_AMBIENT_ZONE_LIST_STATE_PERSISTENT("AZL_DLC_Hei4_Island_Disabled_Zones", false, true);
            }

            HUD::SET_RADAR_AS_INTERIOR_THIS_FRAME(MISC::GET_HASH_KEY("h4_fake_islandx"), 4700.0f, -5145.0f, 0, 0);
        }
        else if (islandLoaded)
        {
            islandLoaded = false;
            TASK::SET_SCENARIO_GROUP_ENABLED("Heist_Island_Peds", false);
            STREAMING::SET_ISLAND_ENABLED("HeistIsland", false);
            PATHFIND::SET_ALLOW_STREAM_HEIST_ISLAND_NODES(0);
            AUDIO::SET_AMBIENT_ZONE_LIST_STATE_PERSISTENT("AZL_DLC_Hei4_Island_Zones", false, false);
            AUDIO::SET_AMBIENT_ZONE_LIST_STATE_PERSISTENT("AZL_DLC_Hei4_Island_Disabled_Zones", true, false);
        }

        IsPlayerInMissionStartRange();
        ProcessTriggerPeds();

        if (SURVIVAL::SurvivalData::IsActive) {
            if (SURVIVAL::SurvivalData::Started) {
                SURVIVAL::ProcessSurvival();

                if (PLAYER::IS_PLAYER_DEAD(0)) {
                    SURVIVAL::QuitSurvival(true);
                }
            }
        }
        else
        {
            processMarkers();
        }

        ControlsWatch();
        WAIT(0);
    }
}

#pragma clang diagnostic pop

void ScriptMain() {
    srand(GetTickCount64());
    main();
}

void OnAbort() {
    ClearAllies(true);
    PLAYER::SET_PLAYER_CONTROL(playerId, true, 0);
    TASK::CLEAR_PED_TASKS(PLAYER::PLAYER_PED_ID());
    PLAYER::SET_DISPATCH_COPS_FOR_PLAYER(playerId, true);
    TriggerPedsData::ClearTriggerPeds();

    if (SURVIVAL::SurvivalData::IsActive) {
        SURVIVAL::ScriptQuit();
    }
    
    if (!entranceBlips.empty()) {
        for (Blip& blip : entranceBlips) {
            HUD::REMOVE_BLIP(&blip);
        }
        
        entranceBlips.clear();
    }

    UnloadNY();
    UnloadBunker();
    UnloadCayoPerico();

    if (STREAMING::IS_IPL_ACTIVE("gr_case6_bunkerclosed"))
    {
        STREAMING::REMOVE_IPL("gr_case6_bunkerclosed");
    }

    DLC::ON_ENTER_SP();
    MISC::SET_INSTANCE_PRIORITY_MODE(0);
}
