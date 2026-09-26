import useLibraryStore from "@/stores/libraryStore";
import { sendToBackend, subscribeToBackend } from "@/lib/bridge";

export const requestLibrary = () => sendToBackend({ type: "get-library" });

export const openImportDialog = () =>
	sendToBackend({ type: "open-import-dialog" });

export const openNewSongsInPicard = () =>
	sendToBackend({ type: "open-new-songs-in-picard" });

export const importFiles = (files: string[]) =>
	sendToBackend({ type: "library-import-files", files });

export const getCombinedSize = () =>
	sendToBackend({ type: "get-combined-size" });

export const initializeLibraryBridge = () => {
	return subscribeToBackend((message) => {
		switch (message.type) {
			case "library-import-result":
				console.log(`Imported ${message.imported} song(s)`);
				break;

			case "library-songs":
				useLibraryStore.getState().setSongs(message.songs);
				break;

			case "combined-size":
				console.log(message)
				useLibraryStore.setState({ combinedSize: message.size });
				break;
		}
	});
};

