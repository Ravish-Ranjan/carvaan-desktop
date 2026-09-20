#pragma once

#include <cstdint>
#include <string>

enum class SongStatus {
    NEW,
    SYNCED,
    PENDING_DELETE,
    MISSING_LOCAL,
    MISSING_PHONE,
    MODIFIED,
    ERROR
};

struct Song {
    std::int64_t id = 0;
    std::string name;
    std::string filepath;
    std::int64_t filesize = 0;
    SongStatus status = SongStatus::NEW;
};

std::string songStatusToString(SongStatus status);

SongStatus songStatusFromString(const std::string& status);