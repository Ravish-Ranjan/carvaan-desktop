import useLibraryStore from "@/stores/libraryStore";
import { sendToBackend, subscribeToBackend } from "@/lib/bridge";

export const requestLibrary = () => sendToBackend({ type: "get-library" });

export const openImportDialog = () =>
	sendToBackend({ type: "open-import-dialog" });

export const importFiles = (files: string[]) =>
	sendToBackend({ type: "library-import-files", files });

export const initializeLibraryBridge = () => {
	return subscribeToBackend((message) => {
		switch (message.type) {
			case "library-import-result":
				console.log(`Imported ${message.imported} song(s)`);
				break;

			case "library-songs":
				useLibraryStore.getState().setSongs(message.songs);
				break;
		}
	});
};
