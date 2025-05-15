#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

//line 71
const f32 minHopSpeed = 6.0;

typedef struct struct_8085C2A4 {
    /* 0x0 */ PlayerAnimationHeader* unk_0;
    /* 0x4 */ PlayerAnimationHeader* unk_4;
    /* 0x8 */ PlayerAnimationHeader* unk_8;
} struct_8085C2A4; // size = 0xC

extern FloorProperty sPrevFloorProperty;
extern struct_8085C2A4 D_8085C2A4[];
extern PlayerAnimationHeader gPlayerAnim_link_normal_jump;
extern PlayerAnimationHeader gPlayerAnim_link_normal_run_jump;

extern void func_80834D50(PlayState* play, Player* this, PlayerAnimationHeader* anim, f32 speed, u16 sfxId);
extern void Player_PlaySfx(Player* player, u16 sfxId);
extern void Player_AnimSfx_PlayVoice(Player* this, u16 sfxId);
extern void func_808373A4(PlayState* play, Player* this);

RECOMP_PATCH s32 func_808373F8(PlayState* play, Player* this, u16 sfxId) {
    PlayerAnimationHeader* anim;
    f32 speed;
    s16 yawDiff = this->yaw - this->actor.shape.rot.y;

    if ((IREG(66) / 100.0f) < this->speedXZ) {
        speed = IREG(67) / 100.0f;
    } else {
        speed = (IREG(68) / 100.0f + (IREG(69) * this->speedXZ) / 1000.0f);

        if ((this->transformation == PLAYER_FORM_DEKU) && (speed < 8.0f)) {
            speed = 8.0f;
        } else if (speed < 5.0f) {
            speed = 5.0f;
        }
    }

    if ((ABS_ALT(yawDiff) >= 0x1000) || (this->speedXZ <= 4.0f)) {
        anim = &gPlayerAnim_link_normal_jump;
    } else {
        s32 var_v1;

        if ((this->transformation != PLAYER_FORM_DEKU) &&
            ((sPrevFloorProperty == FLOOR_PROPERTY_1) || (sPrevFloorProperty == FLOOR_PROPERTY_2))) {
            if (sPrevFloorProperty == FLOOR_PROPERTY_1) {
                var_v1 = 4;
            } else {
                var_v1 = 5;
            }

            func_80834D50(play, this, D_8085C2A4[var_v1].unk_0, speed, ((var_v1 == 4) ? NA_SE_VO_LI_SWORD_N : sfxId));
            this->av2.actionVar2 = -1;
            this->stateFlags2 |= PLAYER_STATE2_80000;
            this->av1.actionVar1 = var_v1;
            return true;
        }
        anim = &gPlayerAnim_link_normal_run_jump;
    }

    // Deku hopping
    if (this->transformation == PLAYER_FORM_DEKU) {
        speed *= 0.3f + ((5 - this->remainingHopsCounter) * 0.18f);
        if (speed < 4.0) {
            speed = 4.0;
        }
        
        if ((this->actor.depthInWater > 0.0f) && this->remainingHopsCounter == 5 && this->speedXZ < minHopSpeed) {
            this->speedXZ = minHopSpeed;
        }

        if ((this->actor.depthInWater > 0.0f) && (this->remainingHopsCounter != 0)) {
            this->actor.world.pos.y += this->actor.depthInWater;
            func_80834D50(play, this, anim, speed, NA_SE_NONE);
            this->av2.actionVar2 = 1;
            this->stateFlags3 |= PLAYER_STATE3_200000;
            Player_PlaySfx(this, (NA_SE_PL_DEKUNUTS_JUMP5 + 1 - this->remainingHopsCounter));
            Player_AnimSfx_PlayVoice(this, sfxId);
            this->remainingHopsCounter--;
            if (this->remainingHopsCounter == 0) {
                this->stateFlags2 |= PLAYER_STATE2_80000;
                func_808373A4(play, this);
            }

            return true;
        }

        if (this->actor.velocity.y > 0.0f) {
            sfxId = NA_SE_NONE;
        }
    }

    func_80834D50(play, this, anim, speed, sfxId);
    this->av2.actionVar2 = 1;

    return true;
}