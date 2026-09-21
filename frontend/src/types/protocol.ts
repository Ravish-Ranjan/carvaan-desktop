import type { Song } from "@/types/song";

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

export type FrontendMessage =
	| FrontendReadyMessage
	| GetLibraryMessage
	| OpenImportDialogMessage
	| LibraryImportFilesMessage;

// ? C++ → Frontend

export interface LibrarySongsMessage {
	type: "library-songs";
	songs: Song[];
}

export interface LibraryImportResultMessage {
	type: "library-import-result";
	imported: number;
}

export type BackendMessage = LibrarySongsMessage | LibraryImportResultMessage;
