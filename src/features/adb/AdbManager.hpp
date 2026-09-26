#pragma once

#include "../../core/Song.hpp"

class AdbManager {
    public:
        bool isDeviceConnected() const;
        bool deleteSong(const Song& song);
};