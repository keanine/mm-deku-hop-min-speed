#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

#define CFG_MIN_SPEED recomp_get_config_double("min_speed")

RECOMP_HOOK("func_808373F8") s32 func_808373F8(PlayState* play, Player* this, u16 sfxId) {
    if ((this->actor.depthInWater > 0.0f) && this->remainingHopsCounter == 5 && this->speedXZ < CFG_MIN_SPEED) {
        this->speedXZ = CFG_MIN_SPEED;
    }
}