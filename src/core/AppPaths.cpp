#include "AppPaths.hpp"

#include <cstdlib>

namespace AppPaths {
    std::filesystem::path dataDirectory() {
        const char* xdgDataHome = std::getenv("XDG_DATA_HOME");

        if (xdgDataHome != nullptr && *xdgDataHome != '\0') {
            return std::filesystem::path(xdgDataHome) / "sync-songs-app";
        }

        const char* home = std::getenv("HOME");

        if (home == nullptr || *home == '\0') return {};

        return std::filesystem::path(home) / ".local" / "share" / "sync-songs-app";
    }

    std::filesystem::path databaseFile(){ 
        return dataDirectory() / "sync-songs.db";
    }
}