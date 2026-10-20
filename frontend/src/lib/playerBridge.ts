import usePlayerStore from "@/stores/playerStore";
import { sendToBackend, subscribeToBackend } from "@/lib/bridge";
import { getMetadata } from "./songs";

let requestedSongId: number | null = null;

export const playSong = (id: number) => {
	requestedSongId = id;

	sendToBackend({
		type: "play-song",
		id,
	});
};

export const initializePlayerBridge = () => {
	return subscribeToBackend((message) => {
		switch (message.type) {
			case "song-playing": {
				if (message.id !== requestedSongId) {
					console.debug(
						"[Player] Ignoring stale song response:",
						message.id,
					);
					break;
				}

				if (!message.fileurl) {
					console.error(
						"[Player] C++ returned an empty media URL",
						message,
					);
					break;
				}

				const requestedId = message.id;
				const fileUrl = message.fileurl;

				console.log("[Player] Loading song:", fileUrl);

				getMetadata(fileUrl, requestedId)
					.then((metadata) => {
						if (requestedSongId !== requestedId) {
							console.debug(
								"[Player] Ignoring stale metadata:",
								requestedId,
							);

							if (metadata.coverArt) {
								URL.revokeObjectURL(metadata.coverArt);
							}

							return;
						}

						usePlayerStore.getState().setPlayer({
							...metadata,
							url: fileUrl,
						});

						console.log("[Player] Metadata loaded:", metadata);
					})
					.catch((error: unknown) => {
						if (requestedSongId !== requestedId) return;

						console.error(
							"[Player] Failed to load song metadata:",
							fileUrl,
							error,
						);
					});

				break;
			}
		}
	});
};
