#include "SettingsRepository.hpp"

SettingsRepository::SettingsRepository(Database& database): database_(database) {}

std::optional<std::string> SettingsRepository::get(const std::string& key) {
    const char* sql = R"SQL(SELECT value FROM settings WHERE key = ?;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(statement,1,key.c_str(),-1,SQLITE_TRANSIENT);
    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    const char* value = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
    if (value == nullptr) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    std::string result = value;
    sqlite3_finalize(statement);

    return result;
}

bool SettingsRepository::set(const std::string& key,const std::string& value) {
    const char* sql = R"SQL(INSERT INTO settings (key, value) VALUES (?, ?) ON CONFLICT(key) DO UPDATE SET value = excluded.value; )SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement,1,key.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(statement,2,value.c_str(),-1,SQLITE_TRANSIENT);

    const bool success = sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_finalize(statement);

    return success;
}

std::optional<Settings> SettingsRepository::getSettings() {
    const auto phoneDestination = get("phone_destination");
    const auto pcMusicDestination = get("pc_music_destination");

    if (!phoneDestination.has_value() ||!pcMusicDestination.has_value()) return std::nullopt;

    Settings settings;

    settings.phoneDestination = *phoneDestination;
    settings.pcMusicDestination = *pcMusicDestination;

    return settings;
}


