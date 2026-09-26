#pragma once

#include "Database.hpp"
#include "Settings.hpp"

#include <optional>
#include <string>

class SettingsRepository {
public:
    explicit SettingsRepository(Database& database);

    std::optional<std::string> get(const std::string& key);
    bool set(const std::string& key,const std::string& value);
    std::optional<Settings> getSettings();

private:
    Database& database_;
};