import { create } from "zustand";
import { SONG_STATUSES, type Song, type SongRaw } from "@/types/song";

interface LibraryStoreProps {
	songs: Song[];
	isLoading: boolean;
	combinedSize: string;

	setSongs: (rawsongs: SongRaw[]) => void;
	addSongs: (songs: Song[]) => void;
	removeSong: (id: number) => void;
	clearSongs: () => void;
	setLoading: (loading: boolean) => void;
}

const useLibraryStore = create<LibraryStoreProps>((set) => ({
	songs: [],
	isLoading: false,
	combinedSize: "0 B",

	setSongs: (rawsongs) => {
		const songs: Song[] = rawsongs.map((song) => {
			return { ...song, status: SONG_STATUSES[song.status] };
		});
		set({ songs });
	},

	addSongs: (songs) =>
		set((state) => ({ songs: [...state.songs, ...songs] })),

	removeSong: (id) =>
		set((state) => ({
			songs: state.songs.filter((song) => song.id !== id),
		})),

	clearSongs: () => set({ songs: [] }),

	setLoading: (isLoading) => set({ isLoading }),
}));

export default useLibraryStore;
