import { sendToBackend } from "@/lib/bridge";
import type { Metadata, QueueItem } from "@/types/song";
import { create } from "zustand";

export type RepeatMode = "off" | "all" | "one";

interface PlayerStoreProps {
	id?: number;
	url?: string;
	title: string;
	duration?: number;
	coverArt?: string;
	artist?: string;
	album?: string;
	year?: number;

	isPlaying: boolean;
	currentTime: number;
	volume: number;
	isMuted: boolean;
	shuffle: boolean;
	repeat: RepeatMode;

	queue: QueueItem[];
	queueIndex: number;

	setPlayer: (metadata: Metadata) => void;
	clearPlayer: () => void;

	setIsPlaying: (playing: boolean) => void;
	setCurrentTime: (time: number) => void;
	setDuration: (duration: number) => void;
	setVolume: (volume: number) => void;
	setIsMuted: (muted: boolean) => void;
	setShuffle: (shuffle: boolean) => void;
	setRepeat: (repeat: RepeatMode) => void;

	setQueue: (songs: QueueItem[], startIndex?: number) => void;
	addToQueue: (song: QueueItem) => void;
	playFromQueue: (index: number) => void;
}

const initialState = {
	id: undefined,
	url: undefined,
	title: "Unknown",
	duration: undefined,
	coverArt: "",
	artist: "Unknown",
	album: "Unknown",
	year: undefined,

	isPlaying: false,
	currentTime: 0,
	volume: 1,
	isMuted: false,
	shuffle: false,
	repeat: "off" as RepeatMode,

	queue: [] as QueueItem[],
	queueIndex: -1,
};

const usePlayerStore = create<PlayerStoreProps>((set, get) => ({
	...initialState,

	setPlayer: (metadata) =>
		set({
			...metadata,
			currentTime: 0,
			isPlaying: false,
		}),

	clearPlayer: () => set({ ...initialState }),

	setIsPlaying: (isPlaying) => set({ isPlaying }),
	setCurrentTime: (currentTime) => set({ currentTime }),
	setDuration: (duration) => set({ duration }),
	setVolume: (volume) => set({ volume }),
	setIsMuted: (isMuted) => set({ isMuted }),
	setShuffle: (shuffle) => set({ shuffle }),
	setRepeat: (repeat) => set({ repeat }),

	setQueue: (songs, startIndex = 0) => {
		if (songs.length === 0) {
			set({ queue: [], queueIndex: -1 });
			return;
		}

		const safeIndex = Math.max(0, Math.min(startIndex, songs.length - 1));

		set({
			queue: songs,
			queueIndex: safeIndex,
		});
	},

	addToQueue: (song) => {
		set((state) => ({
			queue: [...state.queue, song],
		}));
	},

	playFromQueue: (index) => {
		const { queue } = get();

		if (index < 0 || index >= queue.length) return;

		// Update the index before requesting the song.
		// The actual player state will be updated when
		// the backend responds with "song-playing".
		set({ queueIndex: index });
		sendToBackend({
			type: "play-song",
			id: queue[index].id,
		});
	},
}));

export default usePlayerStore;
