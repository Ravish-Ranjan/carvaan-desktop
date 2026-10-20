import type { Metadata } from "@/types/song";
import { create } from "zustand";

interface PlayerStoreProps {
	id?: number;
	url?: string;
	title: string;
	duration?: number;
	coverArt?: string;
	artist?: string;
	album?: string;
	year?: number;

	clearPlayer: () => void;
	setPlayer: (metadata: Metadata) => void;
}

const initstate = {
	id: undefined,
	url: undefined,
	title: "Unknown",
	duration: undefined,
	coverArt: "",
	artist: "Unknown",
	album: "Unknown",
	year: undefined,
};

const usePlayerStore = create<PlayerStoreProps>((set) => ({
	...initstate,
	clearPlayer: () => set({ ...initstate }),
	setPlayer: (metadata) => set({ ...metadata }),
}));

export default usePlayerStore;
