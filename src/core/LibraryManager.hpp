#pragma once

#include "Song.hpp"

#include <filesystem>
#include <vector>

class SongRepository;

class LibraryManager {
    public:
        explicit LibraryManager(SongRepository& repository);
        bool importFile(const std::filesystem::path& filepath);
        std::size_t importFiles(const std::vector<std::filesystem::path>& files);
        std::size_t scanDirectory(const std::filesystem::path& directory);
        std::vector<Song> getSongs();
        std::int64_t getCombinedSongsSize();
        void refreshFilesize();
        void deleteTrack(std::int64_t id);
        std::vector<Song> getNewSongs();

    private:
        SongRepository& repository_;
};