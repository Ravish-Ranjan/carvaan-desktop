import usePlayerStore from "@/stores/playerStore";
import { sendToBackend, subscribeToBackend } from "@/lib/bridge";
import { getMetadata } from "./songs";

export const playSong = (id: number) => {
	sendToBackend({
		type: "play-song",
		id,
	});
};

export const initializePlayerBridge = () => {
	return subscribeToBackend((message) => {
		switch (message.type) {
			case "song-playing":
				if (!message.fileurl) {
					console.error(
						"[Player] C++ returned an empty media URL",
						message,
					);
					break;
				}

				console.log("[Player] Loading song:", message.fileurl);
				getMetadata(message.fileurl, message.id)
					.then((metadata) => {
						usePlayerStore.getState().setPlayer({
							...metadata,
							url: message.fileurl,
						});

						console.log("[Player] Metadata loaded:", metadata);
					})
					.catch((error: unknown) => {
						console.error(
							"[Player] Failed to load song metadata:",
							message.fileurl,
							error,
						);
					});

				break;
		}
	});
};
