import type { LibrarySongsMessage } from "@/types/song";
import useLibraryStore from "@/stores/libraryStore";

export const requestLibrary = () => {
	window.webkit?.messageHandlers?.syncSongs?.postMessage(
		JSON.stringify({ type: "get-library" }),
	);
};

export const initializeLibraryBridge = () => {
	const handleCppMessage = (event: Event) => {
		const customEvent = event as CustomEvent;
		const message = customEvent.detail;

		console.log("[C++]:", message);

		if (message?.type === "library-songs") {
			const libraryMessage = message as LibrarySongsMessage;

			useLibraryStore.getState().setSongs(libraryMessage.songs);
		}
	};

	window.addEventListener("syncsongs-message", handleCppMessage);

	return () => {
		window.removeEventListener("syncsongs-message", handleCppMessage);
	};
};
