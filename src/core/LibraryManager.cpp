#include "LibraryManager.hpp"
#include "SongRepository.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace {
    bool isSupportedAudioFile(const std::filesystem::path& path) {
        if (!path.has_extension()) return false;

        std::string extension = path.extension().string();
        std::transform(
            extension.begin(),
            extension.end(),
            extension.begin(),
            [](unsigned char character) {
                return static_cast<char>(std::tolower(character));
            }
        );

        return extension == ".mp3" || extension == ".m4a" || extension == ".flac" ||
            extension == ".wav" || extension == ".ogg" || extension == ".opus";
    }

    std::int64_t getFileSize(const std::filesystem::path& path) {
        std::error_code error;
        const auto size = std::filesystem::file_size(path, error);
        if (error) return 0;
        return static_cast<std::int64_t>(size);
    }

    Song createSong(const std::filesystem::path& filepath) {
        Song song;
        song.name = filepath.stem().string();
        song.filepath = std::filesystem::absolute(filepath).string();
        song.filesize = getFileSize(filepath);
        song.status = SongStatus::NEW;
        return song;
    }
}

LibraryManager::LibraryManager(SongRepository& repository): repository_(repository){}

bool LibraryManager::importFile(const std::filesystem::path& filepath) {
    std::error_code error;

    if (!std::filesystem::is_regular_file(filepath,error) || error) return false;
    if (!isSupportedAudioFile(filepath)) return false;
    
    Song song = createSong(filepath);
    
    if (repository_.exists(song.name)) return false;
    
    return repository_.add(song);
}

std::size_t LibraryManager::importFiles(const std::vector<std::filesystem::path>& files) {
    std::size_t imported = 0;

    for (const auto& filepath : files) {
        if (importFile(filepath)) {
            ++imported;
        }
    }

    return imported;
}

std::size_t LibraryManager::scanDirectory(const std::filesystem::path& directory) {
    std::error_code error;

    if (!std::filesystem::is_directory(directory,error) || error) return 0;

    std::size_t imported = 0;

    std::filesystem::recursive_directory_iterator iterator(
        directory,
        std::filesystem::directory_options::skip_permission_denied,
        error
    );

    const std::filesystem::recursive_directory_iterator end;

    while (iterator != end) {
        if (error) {
            error.clear();
            iterator.increment(error);
            continue;
        }

        if (iterator->is_regular_file(error) && !error && isSupportedAudioFile(iterator->path())) {
            if (importFile(iterator->path())) ++imported;
        }

        iterator.increment(error);
    }
    return imported;
}

std::vector<Song> LibraryManager::getSongs() { 
    return repository_.getAll();
}

std::int64_t LibraryManager::getCombinedSongsSize() {
    return repository_.getCombinedSongsSize();
}

void LibraryManager::deleteTrack(std::int64_t id) {
    // repository_.remove(id); mark it delete for sync instead of actual delete if file status is not new
}

std::vector<Song> LibraryManager::getNewSongs() {
    return repository_.getNewSongs();
}

