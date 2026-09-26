#pragma once

namespace Protocol {
    namespace Frontend {
        inline constexpr const char* READY = "frontend-ready";
        inline constexpr const char* GET_LIBRARY = "get-library";
        inline constexpr const char* OPEN_IMPORT_DIALOG = "open-import-dialog";
        inline constexpr const char* LIBRARY_IMPORT_FILES = "library-import-files";
        inline constexpr const char* GET_COMBINED_SIZE = "get-combined-size";
        inline constexpr const char* OPEN_NEW_SONGS_IN_PICARD = "open-new-songs-in-picard";
    }

    namespace Backend {
        inline constexpr const char* LIBRARY_SONGS = "library-songs";
        inline constexpr const char* LIBRARY_IMPORT_RESULT = "library-import-result";
        inline constexpr const char* COMBINED_SIZE = "combined-size";
    }
}