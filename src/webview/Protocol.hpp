#pragma once

namespace Protocol {
    namespace Frontend {
        inline constexpr const char* READY = "frontend-ready";
        
        inline constexpr const char* GET_LIBRARY = "get-library";
        inline constexpr const char* OPEN_IMPORT_DIALOG = "open-import-dialog";
        inline constexpr const char* LIBRARY_IMPORT_FILES = "library-import-files";
        inline constexpr const char* GET_COMBINED_SIZE = "get-combined-size";
        inline constexpr const char* OPEN_NEW_SONGS_IN_PICARD = "open-new-songs-in-picard";
        inline constexpr const char* OPEN_PC_MUSIC_DIRECTORY_DIALOG = "open-pc-music-directory-dialog";
        
        inline constexpr const char* GET_SETTINGS = "get-settings";
        inline constexpr const char* GET_PHONE_DESTINATION = "get-phone-destinaton";
        inline constexpr const char* SET_PHONE_DESTINATION = "set-phone-destinaton";
        inline constexpr const char* GET_PC_MUSIC_DESTINATION = "get-phone-destinaton";
        inline constexpr const char* SET_PC_MUSIC_DESTINATION = "set-phone-destinaton";
        
        inline constexpr const char* PLAY_SONG = "play-song";
    }

    namespace Backend {
        inline constexpr const char* LIBRARY_SONGS = "library-songs";
        inline constexpr const char* LIBRARY_IMPORT_RESULT = "library-import-result";
        inline constexpr const char* COMBINED_SIZE = "combined-size";
        inline constexpr const char* PC_MUSIC_DIRECTORY_SELECTED = "pc-music-directory-selected";
        
        inline constexpr const char* SETTINGS = "settings";
        inline constexpr const char* PHONE_DESTINATION = "phone-destination";
        inline constexpr const char* PHONE_DESTINATION_SET = "phone-destination-set";
        inline constexpr const char* PC_MUSIC_DESTINATION = "pc-music-destination";
        inline constexpr const char* PC_MUSIC_DESTINATION_SET = "pc-music-destination-set";
        inline constexpr const char* SETTINGS_VALIDATION = "settings-validation";
        
        inline constexpr const char* SONG_PLAYING = "song-playing";

    }
}