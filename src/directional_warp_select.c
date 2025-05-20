#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

extern u16 sOwlWarpPauseItems[];

typedef enum MapDirection {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} MapDirection;

u16 warpDirectionsVanilla[] = {
    // OWL_WARP_GREAT_BAY_COAST
    OWL_WARP_ZORA_CAPE,
    OWL_WARP_ZORA_CAPE,
    OWL_WARP_IKANA_CANYON,
    OWL_WARP_MILK_ROAD,
    // OWL_WARP_ZORA_CAPE
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_WOODFALL,
    // OWL_WARP_SNOWHEAD
    OWL_WARP_WOODFALL,
    OWL_WARP_MOUNTAIN_VILLAGE,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_STONE_TOWER,
    // OWL_WARP_MOUNTAIN_VILLAGE
    OWL_WARP_SNOWHEAD,
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_STONE_TOWER,
    // OWL_WARP_CLOCK_TOWN
    OWL_WARP_MOUNTAIN_VILLAGE,
    OWL_WARP_WOODFALL,
    OWL_WARP_MILK_ROAD,
    OWL_WARP_IKANA_CANYON,
    // OWL_WARP_MILK_ROAD
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_WOODFALL,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_CLOCK_TOWN,
    // OWL_WARP_WOODFALL
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_SNOWHEAD,
    OWL_WARP_ZORA_CAPE,
    OWL_WARP_SOUTHERN_SWAMP,
    // OWL_WARP_SOUTHERN_SWAMP
    OWL_WARP_IKANA_CANYON,
    OWL_WARP_SOUTHERN_SWAMP,
    OWL_WARP_WOODFALL,
    OWL_WARP_IKANA_CANYON,
    // OWL_WARP_IKANA_CANYON
    OWL_WARP_STONE_TOWER,
    OWL_WARP_SOUTHERN_SWAMP,
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_STONE_TOWER,
    // OWL_WARP_STONE_TOWER
    OWL_WARP_STONE_TOWER,
    OWL_WARP_IKANA_CANYON,
    OWL_WARP_MOUNTAIN_VILLAGE,
    OWL_WARP_GREAT_BAY_COAST
};

u16 warpDirectionsOverhaul[] = {
    // OWL_WARP_GREAT_BAY_COAST
    OWL_WARP_ZORA_CAPE,
    OWL_WARP_ZORA_CAPE,
    OWL_WARP_ZORA_CAPE,
    OWL_WARP_MILK_ROAD,
    // OWL_WARP_ZORA_CAPE
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_STONE_TOWER,
    OWL_WARP_GREAT_BAY_COAST,
    // OWL_WARP_SNOWHEAD
    OWL_WARP_WOODFALL,
    OWL_WARP_MOUNTAIN_VILLAGE,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_IKANA_CANYON,
    // OWL_WARP_MOUNTAIN_VILLAGE
    OWL_WARP_SNOWHEAD,
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_IKANA_CANYON,
    // OWL_WARP_CLOCK_TOWN
    OWL_WARP_MOUNTAIN_VILLAGE,
    OWL_WARP_SOUTHERN_SWAMP,
    OWL_WARP_MILK_ROAD,
    OWL_WARP_IKANA_CANYON,
    // OWL_WARP_MILK_ROAD
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_SOUTHERN_SWAMP,
    OWL_WARP_GREAT_BAY_COAST,
    OWL_WARP_CLOCK_TOWN,
    // OWL_WARP_WOODFALL
    OWL_WARP_SOUTHERN_SWAMP,
    OWL_WARP_SNOWHEAD,
    OWL_WARP_SOUTHERN_SWAMP,
    OWL_WARP_IKANA_CANYON,
    // OWL_WARP_SOUTHERN_SWAMP
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_WOODFALL,
    OWL_WARP_MILK_ROAD,
    OWL_WARP_WOODFALL,
    // OWL_WARP_IKANA_CANYON
    OWL_WARP_IKANA_CANYON,
    OWL_WARP_IKANA_CANYON,
    OWL_WARP_CLOCK_TOWN,
    OWL_WARP_STONE_TOWER,
    // OWL_WARP_STONE_TOWER
    OWL_WARP_STONE_TOWER,
    OWL_WARP_STONE_TOWER,
    OWL_WARP_IKANA_CANYON,
    OWL_WARP_ZORA_CAPE
};

#define CFG_MAP_TYPE recomp_get_config_u32("map_type")
#define CFG_MOD_ENABLED (bool)recomp_get_config_u32("mod_enabled")

PlayState* gPlay;
PauseContext* gPauseCtx;
OwlWarpId gWarpId = 0;

RECOMP_HOOK("KaleidoScope_UpdateWorldMapCursor") void KaleidoScope_UpdateWorldMapCursor_Init(PlayState* play) {
    gPauseCtx = &play->pauseCtx;
    gWarpId = gPauseCtx->cursorPoint[PAUSE_WORLD_MAP];
    gPlay = play;
}

void MoveCursor(MapDirection direction) {
    OwlWarpId backupWarpId = gWarpId;
    u16 iterations = 0;
    u16 max_iterations = 10;

    gPauseCtx->cursorShrinkRate = 4.0f;

    do {
        if (CFG_MAP_TYPE == 0) {
            gWarpId = warpDirectionsVanilla[(gWarpId * 4) + direction];
        }
        else if (CFG_MAP_TYPE == 1) {
            gWarpId = warpDirectionsOverhaul[(gWarpId * 4) + direction];
        }
        iterations++;
        if (iterations > max_iterations) {
            gWarpId = backupWarpId;
            break;
        }
    } while (!gPauseCtx->worldMapPoints[gWarpId]);

    if (backupWarpId != gWarpId) {
        Audio_PlaySfx(NA_SE_SY_CURSOR);
    }
}

RECOMP_HOOK_RETURN("KaleidoScope_UpdateWorldMapCursor") void KaleidoScope_UpdateWorldMapCursor_Return(PlayState* play) {

    if (gPauseCtx->state != PAUSE_STATE_OWL_WARP_SELECT || !CFG_MOD_ENABLED) {
        return;
    }

    if (gPauseCtx->stickAdjY > 30) {
        MoveCursor(DIRECTION_UP);
    }
    else if (gPauseCtx->stickAdjY < -30) {
        MoveCursor(DIRECTION_DOWN);
    }
    else if (gPauseCtx->stickAdjX < -30) {
        MoveCursor(DIRECTION_LEFT);
    }
    else if (gPauseCtx->stickAdjX > 30) {
        MoveCursor(DIRECTION_RIGHT);
    }

    if (gPauseCtx->stickAdjX != 0 || gPauseCtx->stickAdjY != 0) {
        gPauseCtx->cursorPoint[PAUSE_WORLD_MAP] = gWarpId;

        // Offset from `ITEM_MAP_POINT_GREAT_BAY` is to get the correct ordering in `map_name_static`
        gPauseCtx->cursorItem[PAUSE_MAP] =
        sOwlWarpPauseItems[gPauseCtx->cursorPoint[PAUSE_WORLD_MAP]] - ITEM_MAP_POINT_GREAT_BAY;
        // Used as cursor vtxIndex
        gPauseCtx->cursorSlot[PAUSE_MAP] = 31 + gPauseCtx->cursorPoint[PAUSE_WORLD_MAP];
    }
}