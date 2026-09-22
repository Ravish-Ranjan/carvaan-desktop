#include "LibraryFeatures.hpp"

#include "../../core/Song.hpp"
#include "../../webview/Protocol.hpp"

#include <filesystem>
#include <vector>

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



