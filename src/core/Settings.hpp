#pragma once

#include <string>

namespace SettingKey {
    inline constexpr const char* PHONE_DESTINATION = "phone_destination";
    inline constexpr const char* PC_MUSIC_DESTINATION = "pc_music_destination";
}

struct Settings {
    std::string phoneDestination;
    std::string pcMusicDestination;
};