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

    private:
        SongRepository& repository_;
};