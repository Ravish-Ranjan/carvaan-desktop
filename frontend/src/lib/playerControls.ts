import usePlayerStore from "@/stores/playerStore";
import { playSong } from "@/lib/playerBridge";
import { getAudioElement } from "@/lib/audioEngine";

let lastShuffleIndex = -1;

export function playNextTrack() {
	const store = usePlayerStore.getState();
	const { queue, queueIndex, shuffle, repeat } = store;

	if (queue.length === 0 || queueIndex < 0) {
		store.setIsPlaying(false);
		store.setCurrentTime(0);
		return;
	}

	let nextIndex: number;

	if (shuffle && queue.length > 1) {
		do {
			nextIndex = Math.floor(Math.random() * queue.length);
		} while (nextIndex === queueIndex);
	} else if (queueIndex < queue.length - 1) {
		nextIndex = queueIndex + 1;
	} else if (repeat === "all") {
		nextIndex = 0;
	} else {
		const audio = getAudioElement();

		if (audio) {
			audio.pause();
			audio.currentTime = 0;
		}

		store.setIsPlaying(false);
		store.setCurrentTime(0);
		return;
	}

	store.setQueue(queue, nextIndex);
	store.setCurrentTime(0);
	store.setIsPlaying(false);

	playSong(queue[nextIndex].id);
}

export function playPreviousTrack() {
	const store = usePlayerStore.getState();
	const { queue, queueIndex, currentTime, shuffle, repeat } = store;

	const audio = getAudioElement();

	// Restart the current song if it has played for more than 3 seconds.
	if (currentTime > 3) {
		if (audio) {
			audio.currentTime = 0;
			void audio.play().catch((error: unknown) => {
				console.error("[Player] Restart failed:", error);
			});
		}

		store.setCurrentTime(0);
		return;
	}

	if (queue.length === 0 || queueIndex < 0) return;

	let previousIndex: number;

	if (shuffle && queue.length > 1) {
		do {
			previousIndex = Math.floor(Math.random() * queue.length);
		} while (previousIndex === queueIndex);

		lastShuffleIndex = previousIndex;
	} else if (queueIndex > 0) {
		previousIndex = queueIndex - 1;
	} else if (repeat === "all") {
		previousIndex = queue.length - 1;
	} else {
		if (audio) {
			audio.currentTime = 0;
			void audio.play().catch((error: unknown) => {
				console.error("[Player] Restart failed:", error);
			});
		}

		store.setCurrentTime(0);
		return;
	}

	store.setCurrentTime(0);
	store.setIsPlaying(false);
	store.setQueue(queue, previousIndex);
	playSong(queue[previousIndex].id);
}
