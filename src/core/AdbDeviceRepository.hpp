#pragma once

#include "AdbDevice.hpp"
#include "Database.hpp"

#include <vector>
#include <optional>

class AdbDeviceRepository {
public:
    explicit AdbDeviceRepository(Database& database);

    bool save(const AdbDevice& device);
    bool remove(const std::string& serial);
    bool exists(const std::string& serial);
    std::vector<AdbDevice> getAll();
    std::optional<AdbDevice> getBySerial(const std::string& serial);

private:
    Database& database_;
};