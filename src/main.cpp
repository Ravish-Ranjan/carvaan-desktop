#include "webview/WebViewApp.hpp"

#include "core/AppPaths.hpp"
#include "core/Database.hpp"
#include "core/LibraryManager.hpp"
#include "core/SongRepository.hpp"

#include "utils/Json.hpp"

#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]){
    const auto dataDirectory = AppPaths::dataDirectory();

    if (dataDirectory.empty()) {
        std::cerr << "Could not determine application data directory\n";
        return 1;
    }

    std::error_code error;
    std::filesystem::create_directories(dataDirectory,error);

    if (error) {
        std::cerr << "Could not create data directory: " << error.message() << '\n';
        return 1;
    }

    Database database(AppPaths::databaseFile().string());

    if (!database.open()) {
        std::cerr << "Could not open database\n";
        return 1;
    }

    if (!database.initialize()) {
        std::cerr << "Could not initialize database\n";
        return 1;
    }

    SongRepository songRepository(database);
    SettingsRepository settingsRepository(database);

    LibraryManager libraryManager(songRepository);
    
    WebViewApp app(libraryManager,settingsRepository);

    return app.run(argc, argv);
}