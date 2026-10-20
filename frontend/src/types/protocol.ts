import type { SongRaw } from "@/types/song";

// ? Frontend → C++

export interface FrontendReadyMessage {
	type: "frontend-ready";
}

export interface GetLibraryMessage {
	type: "get-library";
}

export interface OpenImportDialogMessage {
	type: "open-import-dialog";
}

export interface LibraryImportFilesMessage {
	type: "library-import-files";
	files: string[];
}

export interface GetCombinedSize {
	type: "get-combined-size";
}

export interface OpenNewSongsInPicard {
	type: "open-new-songs-in-picard";
}

export interface OpenPcMusicDirectoryDialogMessage {
	type: "open-pc-music-directory-dialog";
}

export interface GetSettings {
	type: "get-settings";
}

export interface SetPhoneDestination {
	type: "set-phone-destination";
	path: string;
}

export interface GetPhoneDestination {
	type: "get-phone-destination";
}

export interface SetPcMusicDestination {
	type: "set-pc-music-destination";
	path: string;
}

export interface GetPcMusicDestination {
	type: "get-pc-music-destination";
}

export interface ValidateSettings {
	type: "validate-settings";
}

export interface PlaySong {
	type: "play-song";
	id: number;
}

export type FrontendMessage =
	| FrontendReadyMessage
	| GetLibraryMessage
	| OpenImportDialogMessage
	| LibraryImportFilesMessage
	| GetCombinedSize
	| OpenNewSongsInPicard
	| OpenPcMusicDirectoryDialogMessage
	| GetSettings
	| SetPhoneDestination
	| GetPhoneDestination
	| SetPcMusicDestination
	| GetPcMusicDestination
	| ValidateSettings
	| PlaySong;

// ? C++ → Frontend

export interface LibrarySongsMessage {
	type: "library-songs";
	songs: SongRaw[];
}

export interface LibraryImportResultMessage {
	type: "library-import-result";
	imported: number;
}

export interface CombinedSize {
	type: "combined-size";
	size: string;
}

export interface PcMusicDirectorySelectedMessage {
	type: "pc-music-directory-selected";
	path: string;
}

export interface Settings {
	type: "settings";
	phoneDestination: string;
	pcMusicDestination: string;
}

export interface PhoneDestination {
	type: "phone-destination";
	path: string;
}

export interface PhoneDestinationSet {
	type: "phone-destination-set";
	path: string;
}

export interface PcMusicDestination {
	type: "pc-music-destination";
	path: string;
}

export interface PcMusicDestinationSet {
	type: "pc-music-destination-set";
	path: string;
}

export interface SettingsValidation {
	type: "settings-validation";
	valid: boolean;
}

export interface SongPlaying {
	type: "song-playing";
	id: number;
	fileurl: string;
}

export type BackendMessage =
	| LibrarySongsMessage
	| LibraryImportResultMessage
	| CombinedSize
	| PcMusicDirectorySelectedMessage
	| Settings
	| PhoneDestination
	| PhoneDestinationSet
	| PcMusicDestination
	| PcMusicDestinationSet
	| SettingsValidation
	| SongPlaying;
