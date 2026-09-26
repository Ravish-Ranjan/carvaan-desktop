#pragma once

#include "Song.hpp"

#include <filesystem>
#include <vector>

class SongRepository;

enum class DeleteAction {
    REMOVE_LOCAL,
    REMOVE_FROM_PHONE,
    INVALID
};

enum class DeleteResult {
    DELETED,
    PHONE_DELETION_REQUIRED,
    INVALID
};

class LibraryManager {
    public:
        explicit LibraryManager(SongRepository& repository);
        bool importFile(const std::filesystem::path& filepath);
        std::size_t importFiles(const std::vector<std::filesystem::path>& files);
        std::size_t scanDirectory(const std::filesystem::path& directory);
        std::vector<Song> getSongs();
        std::int64_t getCombinedSongsSize();
        void refreshFilesize();
        DeleteAction getDeleteAction(std::int64_t id);
        DeleteResult deleteTrack(std::int64_t id);
        std::vector<Song> getNewSongs();
        std::optional<Song> getById(std::int64_t id);

    private:
        SongRepository& repository_;
};