#include "LibraryFeatures.hpp"

#include "../../core/Song.hpp"
#include "../../webview/Protocol.hpp"

#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <cmath>

LibraryFeatures::LibraryFeatures(
    LibraryManager& libraryManager,
    SendToReact sendToReact
): libraryManager_(libraryManager), sendToReact_(sendToReact){};

void LibraryFeatures::sendLibrarySongs() {
    const auto songs = libraryManager_.getSongs();

    nlohmann::json songsArray = nlohmann::json::array();

    for (const Song& song : songs) {
        Json jsonSong = songToJson(song);
        songsArray.push_back(std::move(jsonSong));
    }

    Json message = {
        {"type",Protocol::Backend::LIBRARY_SONGS},
        {"songs",std::move(songsArray)}
    };

    sendToReact_(message);
}

void LibraryFeatures::openImportFileChooser(GtkWindow* parentWindow) {
    GtkWidget* dialog =
        gtk_file_chooser_dialog_new(
            "Import Songs",
            GTK_WINDOW(parentWindow),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancel",
            GTK_RESPONSE_CANCEL,
            "_Open",
            GTK_RESPONSE_ACCEPT,
            nullptr
        );

    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),TRUE);

    GtkFileFilter* audioFilter = gtk_file_filter_new();
    gtk_file_filter_set_name(audioFilter,"Audio Files");
    gtk_file_filter_add_mime_type(audioFilter,"audio/mpeg");
    gtk_file_filter_add_mime_type(audioFilter,"audio/mp4");
    gtk_file_filter_add_mime_type(audioFilter,"audio/flac");
    gtk_file_filter_add_mime_type(audioFilter,"audio/wav");
    gtk_file_filter_add_mime_type(audioFilter,"audio/ogg");
    gtk_file_filter_add_mime_type(audioFilter,"audio/opus");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),audioFilter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GSList* selectedFiles = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));

        std::vector<std::filesystem::path> files;

        for (GSList* item = selectedFiles;item != nullptr;item = item->next) {
            const char* filename = static_cast<const char*>(item->data);
            if (filename != nullptr) files.emplace_back(filename);
            g_free(item->data);
        }

        g_slist_free(selectedFiles);
        const std::size_t imported = libraryManager_.importFiles(files);
        g_print("Imported %zu song(s)\n",imported);

        sendToReact_(Json{
            {"type",Protocol::Backend::LIBRARY_IMPORT_RESULT},
            {"imported",imported}
        });
    }

    gtk_widget_destroy(dialog);
}

void LibraryFeatures::handleLibraryImport(const Json& message) {
    if (!message.contains("files")) {
        g_warning("[Library] Missing files");
        return;
    }

    const Json& filesJson = message.at("files");

    if (!filesJson.is_array()) {
        g_warning("[Library] files must be an array");
        return;
    }

    std::vector<std::filesystem::path> files;

    files.reserve(filesJson.size());

    for (const auto& file : filesJson) {
        if (!file.is_string()) {
            g_warning("[Library] File path must be a string");
            return;
        }

        files.emplace_back(file.get<std::string>());
    }

    const std::size_t imported = libraryManager_.importFiles(files);

    g_print("[Library] Imported %zu song(s)\n",imported);

    sendToReact_(Json{
        {"type",Protocol::Backend::LIBRARY_IMPORT_RESULT},
        {"imported",imported}
    });
}

void LibraryFeatures::openNewSongsInPicard() {
    std::vector<Song> newSongs = libraryManager_.getNewSongs();

    if (newSongs.empty()) return;

    std::string cmd = "picard";

    for(const Song& song: newSongs) {
        cmd += " \"" + song.filepath + "\"";
    }

    std::system(cmd.c_str());
}

void LibraryFeatures::deleteTrack(std::int64_t id) {
    libraryManager_.deleteTrack(id);
}

void LibraryFeatures::getCombinedSongsSize() {
    std::int64_t combinedSongsSize = libraryManager_.getCombinedSongsSize();

    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;
    
    std::ostringstream size;

    if (combinedSongsSize < KB) {
        size << combinedSongsSize << " B";
    } else if (combinedSongsSize < MB) {
        size << std::fixed << std::setprecision(2) << combinedSongsSize / KB << " KB";
    } else if (combinedSongsSize < GB) {
        size << std::fixed << std::setprecision(2) << combinedSongsSize / MB << " MB";
    } else if (combinedSongsSize < TB) {
        size << std::fixed << std::setprecision(2) << combinedSongsSize / GB << " GB";
    }  else {
        size << std::fixed << std::setprecision(2) << combinedSongsSize / TB << " TB";
    }

    sendToReact_(Json{
        {"type",Protocol::Backend::COMBINED_SIZE},
        {"size",size.str()}
    });
}

void LibraryFeatures::refreshFilesize() {
    
}



