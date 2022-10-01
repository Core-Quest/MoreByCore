#pragma once

#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/Color.hpp"

DECLARE_CONFIG(MainConfig,
    CONFIG_VALUE(isSaberMarqEnabled, bool, "Saber Marqs", false, "Enable Saber Marqs");
    CONFIG_VALUE(NoPromo, bool, "No Promo", false, "Turn off Promo");
    CONFIG_VALUE(lightsEnable, bool, "Enable Lights Ingame", false, "Enable Lights");
    CONFIG_VALUE(updatelights_preview, bool, "Enabled", false, "Enable Lights Preview");
    CONFIG_VALUE(marq_Intensity, bool, "Intensity", 0.5f, "How Intense you want you saber Marqs");
    CONFIG_VALUE(MenuClr, UnityEngine::Color, "Color", UnityEngine::Color(1.0, 1.0, 1.0, 0.75));
    CONFIG_VALUE(BombExplosions, bool, "Bomb Explosion Effects", false, "Remove Bomb Explosions")
    CONFIG_VALUE(MainMenuSettings_Enabled, bool, "MainMenu Settings", false, "Open MainMenu Settings")
    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(isSaberMarqEnabled);
        CONFIG_INIT_VALUE(NoPromo);
        CONFIG_INIT_VALUE(marq_Intensity);
        CONFIG_INIT_VALUE(MenuClr);
        CONFIG_INIT_VALUE(BombExplosions);
        CONFIG_INIT_VALUE(MainMenuSettings_Enabled);
        CONFIG_INIT_VALUE(lightsEnable);
        CONFIG_INIT_VALUE(updatelights_preview);

    )
)