#include "Song.hpp"

#include <stdexcept>

std::string songStatusToString(SongStatus status){
    switch (status) {
        case SongStatus::NEW:
            return "NEW";

        case SongStatus::SYNCED:
            return "SYNCED";

        case SongStatus::PENDING_DELETE:
            return "PENDING_DELETE";

        case SongStatus::MISSING_LOCAL:
            return "MISSING_LOCAL";

        case SongStatus::MISSING_PHONE:
            return "MISSING_PHONE";

        case SongStatus::MODIFIED:
            return "MODIFIED";

        case SongStatus::ERROR:
            return "ERROR";
    }

    throw std::invalid_argument("Unknown song status");
}

SongStatus songStatusFromString(const std::string& status){
    if (status == "NEW") return SongStatus::NEW;
    if (status == "SYNCED") return SongStatus::SYNCED;
    if (status == "PENDING_DELETE") return SongStatus::PENDING_DELETE;
    if (status == "MISSING_LOCAL") return SongStatus::MISSING_LOCAL;
    if (status == "MISSING_PHONE") return SongStatus::MISSING_PHONE;
    if (status == "MODIFIED") return SongStatus::MODIFIED;
    if (status == "ERROR") return SongStatus::ERROR;

    throw std::invalid_argument(
        "Unknown song status: " + status
    );
}

Json songToJson(const Song& song) {
    return {
        {"id",song.id},
        {"name",song.name},
        {"filepath",song.filepath},
        {"filesize",song.filesize},
        {"status",song.status}
    };
}

Song jsonToSong(const Json& json) {
    Song song;

    song.id = json.at("id").get<std::int64_t>();
    song.name = json.at("name").get<std::string>();
    song.filepath = json.at("filepath").get<std::string>();
    song.filesize = json.at("filesize").get<std::int64_t>();
    song.status = songStatusFromString(json.at("status").get<std::string>());

    return song;
}