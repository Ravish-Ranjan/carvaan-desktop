#include "AdbManager.hpp"

#include <cstdlib>

bool AdbManager::isDeviceConnected() const {
    return std::system("adb get-state 1>/dev/null 2>/dev/null") == 0;
}

bool AdbManager::deleteSong(const Song& song) {
    if (!isDeviceConnected()) return false;

    const std::string cmd = "";
    return false;
}

