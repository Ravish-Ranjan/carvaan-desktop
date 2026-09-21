#pragma once

namespace Protocol {
    namespace Frontend {
        inline constexpr const char* READY = "frontend-ready";
        inline constexpr const char* GET_LIBRARY = "get-library";
        inline constexpr const char* OPEN_IMPORT_DIALOG = "open-import-dialog";
        inline constexpr const char* LIBRARY_IMPORT_FILES = "library-import-files";
    }

    namespace Backend {
        inline constexpr const char* LIBRARY_SONGS = "library-songs";
        inline constexpr const char* LIBRARY_IMPORT_RESULT = "library-import-result";
    }
}