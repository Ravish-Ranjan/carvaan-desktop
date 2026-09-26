#include "SongRepository.hpp"
#include "Database.hpp"

#include <sqlite3.h>

SongRepository::SongRepository(Database& database): database_(database){}

bool SongRepository::add(const Song& song) {
    const char* sql = R"SQL(INSERT INTO songs (name, filepath, filesize, status) VALUES (?, ?, ?, ?);)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) 
        return false;

    sqlite3_bind_text(statement,1,song.name.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(statement,2,song.filepath.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement,3,song.filesize);

    const std::string status = songStatusToString(song.status);
    sqlite3_bind_text(statement,4,status.c_str(),-1,SQLITE_TRANSIENT);

    const bool success = sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_finalize(statement);

    return success;
}

bool SongRepository::update(const Song& song) {
    const char* sql = R"SQL(UPDATE songs SET name = ?, filepath = ?, filesize = ?, status = ? WHERE id = ?;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement,1,song.name.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(statement,2,song.filepath.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement,3,song.filesize);
    
    const std::string status = songStatusToString(song.status);
    sqlite3_bind_text(statement,4,status.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement,5,song.id);
    
    const bool success = sqlite3_step(statement) == SQLITE_DONE;

    sqlite3_finalize(statement);

    return success;
}

bool SongRepository::remove(std::int64_t id) {
    const char* sql = "DELETE FROM songs WHERE id = ?;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int64(statement, 1, id);
    if (sqlite3_step(statement) != SQLITE_DONE) {
        sqlite3_finalize(statement);
        return false;
    }

    const bool deleted = sqlite3_changes(database_.handle()) > 0;
    sqlite3_finalize(statement);

    return deleted;
}

std::vector<Song> SongRepository::getAll() {
    std::vector<Song> songs;

    const char* sql = R"SQL(SELECT id, name, filepath, filesize, status FROM songs ORDER BY id COLLATE NOCASE;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return songs;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        Song song;

        song.id = sqlite3_column_int64(statement, 0);
        song.name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        song.filepath = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
        song.filesize = sqlite3_column_int64(statement, 3);
        const char* status = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));

        if (status != nullptr) {
            song.status = songStatusFromString(status);
        }
        songs.push_back(std::move(song));
    }
    sqlite3_finalize(statement);

    return songs;
}

bool SongRepository::exists(const std::string& name) {
    const char* sql = R"SQL(SELECT 1 FROM songs WHERE name = ? LIMIT 1;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(statement,1,name.c_str(),-1,SQLITE_TRANSIENT);

    const bool found = sqlite3_step(statement) == SQLITE_ROW;
    sqlite3_finalize(statement);

    return found;
}

std::vector<Song> SongRepository::getNewSongs() {
    std::vector<Song> songs;

    const char* sql = R"SQL(SELECT id, name, filepath, filesize, status FROM songs WHERE status = 'NEW' ORDER BY name COLLATE NOCASE;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return songs;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        Song song;

        song.id = sqlite3_column_int64(statement, 0);
        song.name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        song.filepath = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
        song.filesize = sqlite3_column_int64(statement, 3);
        const char* status = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));

        if (status != nullptr) {
            song.status = songStatusFromString(status);
        }
        songs.push_back(std::move(song));
    }
    sqlite3_finalize(statement);

    return songs;
}

std::int64_t SongRepository::getCombinedSongsSize() {
    std::int64_t combinedSongsSize = 0;

    const char* sql = R"SQL(SELECT COALESCE(SUM(filesize),0) from songs;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) return combinedSongsSize;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        combinedSongsSize = sqlite3_column_int64(statement, 0);
    }

    sqlite3_finalize(statement);

    return combinedSongsSize;
}

std::optional<Song> SongRepository::getById(std::int64_t id) {
    const char* sql = R"SQL(SELECT id, name, filepath, filesize, status FROM songs WHERE id = ?;)SQL";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(database_.handle(),sql,-1,&statement,nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_int64(statement, 1, id);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    Song song;

    song.id = sqlite3_column_int64(statement, 0);

    const auto* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
    const auto* filepath = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
    const auto filesize = sqlite3_column_int64(statement, 3);
    const auto* status = reinterpret_cast<const char*>(sqlite3_column_text(statement, 4));

    if (name != nullptr) song.name = name;
    if (filepath != nullptr) song.filepath = filepath;
    song.filesize = filesize;
    if (status != nullptr) song.status = songStatusFromString(status);

    sqlite3_finalize(statement);

    return song;
}


