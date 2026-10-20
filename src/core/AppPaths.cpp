#include "AppPaths.hpp"

#include <cstdlib>

namespace AppPaths {
    std::filesystem::path dataDirectory() {
        const char* xdgDataHome = std::getenv("XDG_DATA_HOME");

        if (xdgDataHome != nullptr && *xdgDataHome != '\0') {
            return std::filesystem::path(xdgDataHome) / "carvaan-desktop";
        }

        const char* home = std::getenv("HOME");

        if (home == nullptr || *home == '\0') return {};

        return std::filesystem::path(home) / ".local" / "share" / "carvaan-desktop";
    }

    std::filesystem::path databaseFile(){ 
        return dataDirectory() / "carvaan-desktop.db";
    }

    std::filesystem::path tempSongsDirectory() {
        std::filesystem::path dir = dataDirectory() / "temp-songs";
        std::error_code ec;

        if (!std::filesystem::exists(dir,ec)) {
            if (ec) {
                return {};
            }

            if (!std::filesystem::create_directories(dir, ec) && ec) {
                return {};
            }
        }

        return dir;
    }
}