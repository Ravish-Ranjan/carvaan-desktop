import { create } from "zustand";
import type { Song } from "@/types/song";

interface LibraryStoreProps {
	songs: Song[];
	isLoading: boolean;

	setSongs: (songs: Song[]) => void;
	addSongs: (songs: Song[]) => void;
	removeSong: (id: number) => void;
	clearSongs: () => void;
	setLoading: (loading: boolean) => void;
}

const useLibraryStore = create<LibraryStoreProps>((set) => ({
	songs: [],
	isLoading: false,

	setSongs: (songs) => set({ songs }),

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
