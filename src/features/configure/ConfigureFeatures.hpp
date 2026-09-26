#pragma once

#include "../../core/Settings.hpp"
#include "../../core/SettingsRepository.hpp"
#include "../../utils/Json.hpp"
#include "../../webview/Protocol.hpp"

#include <gtk/gtk.h>
#include <string>

class ConfigureFeatures {
    public:
        using SendToReact = std::function<void(const Json&)>;
        explicit ConfigureFeatures(SettingsRepository& settingsRepository,SendToReact sendToReact);

        void getSettings();
        void getPhoneDestination();
        void setPhoneDestination(const Json& message);
        void getPcMusicDestination();
        void setPcMusicDestination(const Json& message);
        void validateSettings();
        void openPcMusicDirectoryDialog(GtkWindow* parentWindow);

    private:
        SettingsRepository& settingsRepository_;
            SendToReact sendToReact_;
};