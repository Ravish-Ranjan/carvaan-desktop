export const SONG_STATUSES = [
	"NEW",
	"SYNCED",
	"PENDING_DELETE",
	"MISSING_LOCAL",
	"MISSING_PHONE",
	"MODIFIED",
	"ERROR",
] as const;

export type SongStatus = (typeof SONG_STATUSES)[number];

export interface Song {
	id: number;
	name: string;
	filepath: string;
	filesize: number;
	status: SongStatus;
}

export interface SongRaw extends Omit<Song,"status"> {
	status: number;
}
