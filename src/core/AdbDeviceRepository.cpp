#include "AdbDeviceRepository.hpp"

AdbDeviceRepository::AdbDeviceRepository(Database& database): database_(database) {}

bool AdbDeviceRepository::save(const AdbDevice& device) {
    const char* sql = R"SQL(
        INSERT INTO adb_devices (
            serial,
            model,
            manufacturer,
            state
        )
        VALUES (?, ?, ?, ?)
        ON CONFLICT(serial)
        DO UPDATE SET
            model = excluded.model,
            manufacturer = excluded.manufacturer,
            state = excluded.state;
    )SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement, 1,device.serial.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2,device.model.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3,device.manufacturer.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4,device.state.c_str(),-1,SQLITE_TRANSIENT);

    const bool success = sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_finalize(statement);

    return success;
}

bool AdbDeviceRepository::exists(const std::string& serial) {
    const char* sql = R"SQL(SELECT 1 FROM adb_devices WHERE serial = ? LIMIT 1;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement,1,serial.c_str(),-1,SQLITE_TRANSIENT);

    const bool found = sqlite3_step(statement) == SQLITE_ROW;
    sqlite3_finalize(statement);

    return found;
}

std::optional<AdbDevice> AdbDeviceRepository::getBySerial(const std::string& serial) {
    const char* sql = R"SQL(SELECT id,serial,model,manufacturer,state FROM adb_devices WHERE serial = ?;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(statement,1,serial.c_str(),-1,SQLITE_TRANSIENT);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    AdbDevice device;

    device.id = sqlite3_column_int64(statement, 0);

    const char* dbSerial = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
    const char* model = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
    const char* manufacturer = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
    const char* state = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));

    if (dbSerial) device.serial = dbSerial;
    if (model) device.model = model;
    if (manufacturer) device.manufacturer = manufacturer;
    if (state) device.state = state;

    sqlite3_finalize(statement);

    return device;
}

std::vector<AdbDevice> AdbDeviceRepository::getAll() {
    const char* sql = R"SQL(SELECT id, serial, model, manufacturer, state FROM adb_devices ORDER BY id;)SQL";

    sqlite3_stmt* statement = nullptr;

    std::vector<AdbDevice> devices;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return devices;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        AdbDevice device;

        device.id = sqlite3_column_int64(statement, 0);

        const char* serial = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        const char* model = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
        const char* manufacturer = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
        const char* state = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));

        if (serial) device.serial = serial;
        if (model) device.model = model;
        if (manufacturer) device.manufacturer = manufacturer;
        if (state) device.state = state;

        devices.push_back(std::move(device));
    }

    sqlite3_finalize(statement);

    return devices;
}

bool AdbDeviceRepository::remove(const std::string& serial) {
    const char* sql = R"SQL(DELETE FROM adb_devices WHERE serial = ?;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement,1,serial.c_str(),-1,SQLITE_TRANSIENT);

    if (sqlite3_step(statement) != SQLITE_DONE) {
        sqlite3_finalize(statement);
        return false;
    }

    const bool removed = sqlite3_changes(database_.handle()) > 0;
    sqlite3_finalize(statement);

    return removed;
}

