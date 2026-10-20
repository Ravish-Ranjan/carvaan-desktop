#include "PlayerFeatures.hpp"

#include "../../core/AppPaths.hpp"

#include <glib.h>

#include <filesystem>
#include <stdexcept>
#include <string>

PlayerFeatures::PlayerFeatures(LibraryManager& libraryManager,SendToReact sendToReact,MediaUrlProvider mediaUrlProvider)
    : libraryManager_(libraryManager),
      sendToReact_(sendToReact),
      mediaUrlProvider_(mediaUrlProvider) {}

void PlayerFeatures::playSong(const Json& message) {
    if (!message.contains("id")) {
        g_warning("[Player] Missing song id");
        return;
    }

    std::int64_t id;

    try {
        id = message.at("id").get<std::int64_t>();
    } catch (const std::exception& e) {
        g_warning("[Player] Invalid song id: %s", e.what());
        return;
    }

    const std::optional<Song> song = libraryManager_.getById(id);

    if (!song.has_value()) {
        g_warning("[Player] Song not found: %ld", static_cast<long>(id));

        sendToReact_(Json{
            {"type", "song-playing"},
            {"fileurl", ""},
            {"id", id}
        });

        return;
    }

    const std::filesystem::path src = song->filepath;
    const std::filesystem::path tempSongsDir = AppPaths::tempSongsDirectory();
    const std::filesystem::path dest = tempSongsDir / src.filename();

    try {
        if (src.empty() || !std::filesystem::is_regular_file(src)) {
            throw std::runtime_error("Source song file does not exist");
        }

        if (tempSongsDir.empty()) {
            throw std::runtime_error("Could not determine temporary songs directory");
        }

        std::filesystem::create_directories(tempSongsDir);
        std::filesystem::copy_file(src,dest,std::filesystem::copy_options::overwrite_existing);

        const std::string fileurl = mediaUrlProvider_(dest.filename().string());

        if (fileurl.empty()) {
            throw std::runtime_error("Media URL provider returned an empty URL");
        }

        g_message("[Player] Generated media URL: %s", fileurl.c_str());

        sendToReact_(Json{
            {"type", "song-playing"},
            {"fileurl", fileurl},
            {"id", id}
        });
    } catch (const std::exception& e) {
        g_warning("[Player] Failed to prepare song: %s", e.what());

        sendToReact_(Json{
            {"type", "song-playing"},
            {"fileurl", ""},
            {"id", id}
        });
    }
}
