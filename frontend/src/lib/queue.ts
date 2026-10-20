import type { Song, QueueItem } from "@/types/song";
import usePlayerStore from "@/stores/playerStore";
import { playSong } from "@/lib/playerBridge";

export function playLibrarySong(songs: Song[], selectedSongId: number) {
	const selectedIndex = songs.findIndex((song) => song.id === selectedSongId);

	if (selectedIndex === -1) {
		console.error("[Queue] Song not found in library:", selectedSongId);
		return;
	}

	const queue: QueueItem[] = songs.map(({ id }) => ({ id }));

	const store = usePlayerStore.getState();

	store.setQueue(queue, selectedIndex);

	playSong(selectedSongId);
}
