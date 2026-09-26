#pragma once

#include "Song.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

class Database;

class SongRepository {
    public:
        explicit SongRepository(Database& database);
        bool add(const Song& song);
        bool update(const Song& song);
        bool remove(std::int64_t id);
        std::vector<Song> getAll();
        bool exists(const std::string& name);
        std::vector<Song> getNewSongs();
        std::int64_t getCombinedSongsSize();
        std::optional<Song> getById(std::int64_t id);

    private:
        Database& database_;
};