export interface Song {
    id: number;
    name: string;
    filepath: string;
    filesize: string;
    status: string;
}

export interface LibrarySongsMessage {
  type: "library-songs";
  songs: Song[];
}