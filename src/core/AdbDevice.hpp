#pragma once

#include <cstdint>
#include <string>

struct AdbDevice {
    std::int64_t id = 0;

    std::string serial;
    std::string model;
    std::string manufacturer;
    std::string state;
};