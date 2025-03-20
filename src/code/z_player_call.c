#include "global.h"
#include "kaleido_manager.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS |          \
     ACTOR_FLAG_UPDATE_DURING_OCARINA | ACTOR_FLAG_CAN_PRESS_SWITCHES | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

ActorFunc sPlayerCallInitFunc;
ActorFunc sPlayerCallDestroyFunc;
ActorFunc sPlayerCallUpdateFunc;
ActorFunc sPlayerCallDrawFunc;

ActorProfile Player_Profile = {
    /**/ ACTOR_PLAYER,
    /**/ ACTORCAT_PLAYER,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(Player),
    /**/ PlayerCall_Init,
    /**/ PlayerCall_Destroy,
    /**/ PlayerCall_Update,
    /**/ PlayerCall_Draw,
};

void Player_Init(Actor* thisx, PlayState* play);
void Player_Destroy(Actor* thisx, PlayState* play);
void Player_Update(Actor* thisx, PlayState* play);
void Player_Draw(Actor* thisx, PlayState* play);

void SI_ClearWeirdshotData(void);

void PlayerCall_InitFuncPtrs(void) {
    sPlayerCallInitFunc = KaleidoManager_GetRamAddr(Player_Init);
    sPlayerCallDestroyFunc = KaleidoManager_GetRamAddr(Player_Destroy);
    sPlayerCallUpdateFunc = KaleidoManager_GetRamAddr(Player_Update);
    sPlayerCallDrawFunc = KaleidoManager_GetRamAddr(Player_Draw);
}

void PlayerCall_Init(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();

    SI_ClearWeirdshotData();

    PlayerCall_InitFuncPtrs();
    sPlayerCallInitFunc(thisx, play);
}

void PlayerCall_Destroy(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();
    sPlayerCallDestroyFunc(thisx, play);
}

void PlayerCall_Update(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();

    AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
    AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);

    sPlayerCallUpdateFunc(thisx, play);
}

#include "gfxalloc.h"
#include "string.h"

u8 gWeirdshotFrameData[(128 - 24) * (sizeof(Vec3s) * PLAYER_LIMB_MAX + sizeof(s16))];
s32 gWeirdshotFrame;
u32 gWeirdshotSanity;

void SI_ClearWeirdshotData(void) {
    memset(gWeirdshotFrameData, 0xAA, sizeof(gWeirdshotFrameData));
}

void SI_DrawWeirdshotFrame(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;
    GfxPrint printer;
    Gfx* gfxRef;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL28_Opa(play->state.gfxCtx);

    GfxPrint_Init(&printer);

    gfxRef = POLY_OPA_DISP;
    gfx = Gfx_Open(gfxRef);
    gSPDisplayList(OVERLAY_DISP++, gfx);

    GfxPrint_Open(&printer, gfx);

    GfxPrint_SetColor(&printer, 255, 255, 255, 255);

    GfxPrint_SetPos(&printer, 1, 7);
    GfxPrint_Printf(&printer, "data: 0x%08x", (u32)gWeirdshotFrameData);

    GfxPrint_SetPos(&printer, 1, 8);
    GfxPrint_Printf(&printer, "frame: %d", gWeirdshotFrame);

    GfxPrint_SetPos(&printer, 1, 9);
    GfxPrint_Printf(&printer, "sanity: 0x%08x", gWeirdshotSanity);

    gfx = GfxPrint_Close(&printer);

    gSPEndDisplayList(gfx++);
    Gfx_Close(gfxRef, gfx);
    POLY_OPA_DISP = gfx;

    GfxPrint_Destroy(&printer);

    CLOSE_DISPS(play->state.gfxCtx);
}

void PlayerCall_Draw(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();

    SI_DrawWeirdshotFrame(thisx, play);

    sPlayerCallDrawFunc(thisx, play);
}
