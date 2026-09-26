import type {  SongRaw } from "@/types/song";

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

export type FrontendMessage =
	| FrontendReadyMessage
	| GetLibraryMessage
	| OpenImportDialogMessage
	| GetCombinedSize
	| OpenNewSongsInPicard
	| LibraryImportFilesMessage;

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

export type BackendMessage =
	| LibrarySongsMessage
	| LibraryImportResultMessage
	| CombinedSize;
