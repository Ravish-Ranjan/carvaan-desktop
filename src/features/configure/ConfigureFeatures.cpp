#include "ConfigureFeatures.hpp"

#include <filesystem>

ConfigureFeatures::ConfigureFeatures(
    SettingsRepository& settingsRepository,
    SendToReact sendToReact
): settingsRepository_(settingsRepository),sendToReact_(sendToReact) {}

void ConfigureFeatures::getSettings() {
    const auto settings = settingsRepository_.getSettings();

    if (!settings.has_value()) {
        sendToReact_(Json{
            {"type", Protocol::Backend::SETTINGS},
            {"phoneDestination", ""},
            {"pcMusicDestination", ""}
        });

        return;
    }

    sendToReact_(Json{
        {"type", Protocol::Backend::SETTINGS},
        {"phoneDestination", settings->phoneDestination},
        {"pcMusicDestination", settings->pcMusicDestination}
    });
}

void ConfigureFeatures::getPhoneDestination() {
    const auto destination = settingsRepository_.get(SettingKey::PHONE_DESTINATION);

    sendToReact_(Json{
        {"type", Protocol::Backend::PHONE_DESTINATION},
        {"path", destination.value_or("")}
    });
}

void ConfigureFeatures::setPhoneDestination(const Json& message) {
    if (!message.contains("path")) {
        g_warning("[Library] Missing path");
        return;
    }

    const std::string path = message.at("path").get<std::string>();

    if (path.empty() || path.front() != '/') {
        sendToReact_(Json{
            {"type", Protocol::Backend::PHONE_DESTINATION_SET},
            {"path", ""}
        });

        return;
    }

    const bool success = settingsRepository_.set(SettingKey::PHONE_DESTINATION,path);

    sendToReact_(Json{
        {"type", Protocol::Backend::PHONE_DESTINATION_SET},
        {"path", success ? path : ""}
    });
}

void ConfigureFeatures::getPcMusicDestination() {
    const auto destination = settingsRepository_.get(SettingKey::PC_MUSIC_DESTINATION);

    sendToReact_(Json{
        {"type", Protocol::Backend::PC_MUSIC_DESTINATION},
        {"path", destination.value_or("")}
    });
}

void ConfigureFeatures::setPcMusicDestination(const Json& message) {
    if (!message.contains("path")) {
        g_warning("[Library] Missing path");
        return;
    }

    const std::string path = message.at("path").get<std::string>();
    if (path.empty()) {
        sendToReact_(Json{
            {"type", Protocol::Backend::PC_MUSIC_DESTINATION_SET},
            {"path", ""}
        });

        return;
    }

    std::error_code error;
    const std::filesystem::path destination(path);

    if (!destination.is_absolute() ||
        !std::filesystem::exists(destination, error) ||
        !std::filesystem::is_directory(destination, error)) {

        sendToReact_(Json{
            {"type", Protocol::Backend::PC_MUSIC_DESTINATION_SET},
            {"path", ""}
        });

        return;
    }

    const bool success = settingsRepository_.set(SettingKey::PC_MUSIC_DESTINATION,path);

    sendToReact_(Json{
        {"type", Protocol::Backend::PC_MUSIC_DESTINATION_SET},
        {"path", success ? path : ""}
    });
}

void ConfigureFeatures::validateSettings() {
    const auto settings = settingsRepository_.getSettings();

    bool valid = true;

    if (!settings.has_value()) valid = false;
    else {
        if (settings->phoneDestination.empty()) valid = false;
        if (settings->pcMusicDestination.empty()) valid = false;
        if (!settings->phoneDestination.empty() && settings->phoneDestination.front() != '/') 
            valid = false;
        

        std::error_code error;

        const std::filesystem::path pcDestination(settings->pcMusicDestination);

        if (!std::filesystem::exists(pcDestination, error) ||
            !std::filesystem::is_directory(pcDestination, error)) {
            valid = false;
        }
    }

    sendToReact_(Json{
        {"type", Protocol::Backend::SETTINGS_VALIDATION},
        {"valid", valid}
    });
}

void ConfigureFeatures::openPcMusicDirectoryDialog(GtkWindow* parentWindow) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Select Music Directory",
        GTK_WINDOW(parentWindow),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel",
        GTK_RESPONSE_CANCEL,
        "_Select",
        GTK_RESPONSE_ACCEPT,
        nullptr
    );

    const gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        gchar* filename = gtk_file_chooser_get_filename(chooser);

        if (filename != nullptr) {
            sendToReact_(Json{
                {"type", Protocol::Backend::PC_MUSIC_DIRECTORY_SELECTED},
                {"path", filename}
            });

            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);
}


