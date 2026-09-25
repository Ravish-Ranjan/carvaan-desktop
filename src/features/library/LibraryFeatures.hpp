#pragma once

#include "../../core/LibraryManager.hpp"
#include "../../utils/Json.hpp"

#include <gtk/gtk.h>
#include <functional>

class LibraryFeatures {
    public:
        using SendToReact = std::function<void(const Json&)>;

        LibraryFeatures(
            LibraryManager& libraryManager_,
            SendToReact sendToReact
        );

        void sendLibrarySongs();
        void openImportFileChooser(GtkWindow* parentWindow);
        void handleLibraryImport(const Json& message);
        void openNewSongsInPicard();
        void deleteTrack(std::int64_t id);
        std::string getCombinedSongsSize();

    private:
        LibraryManager& libraryManager_;
        SendToReact sendToReact_;

        void refreshFilesize();

};