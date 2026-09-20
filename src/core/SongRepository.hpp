#pragma once

#include "Song.hpp"

#include <cstdint>
#include <string>
#include <vector>

class Database;

class SongRepository {
    public:
        explicit SongRepository(Database& database);
        bool add(const Song& song);
        bool update(const Song& song);
        bool remove(std::int64_t id);
        std::vector<Song> getAll();
        bool exists(const std::string& name);

    private:
        Database& database_;
};