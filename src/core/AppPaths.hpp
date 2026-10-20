#pragma once

#include <filesystem>

namespace AppPaths {
    std::filesystem::path dataDirectory();
    std::filesystem::path databaseFile();
    std::filesystem::path tempSongsDirectory();
}