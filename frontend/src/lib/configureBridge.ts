import { useConfigureStore } from "@/stores/configureStore";
import { sendToBackend, subscribeToBackend } from "./bridge";

export const initializeConfigureBridge = () => {
	return subscribeToBackend((message) => {
		switch (message.type) {
			case "pc-music-directory-selected":
				console.log(message);
				if (typeof message.path === "string") {
					useConfigureStore
						.getState()
						.setPcMusicDestination(message.path);
				}
				break;

			case "settings":
				console.log(message);
				if (
					typeof message.phoneDestination === "string" &&
					typeof message.pcMusicDestination === "string"
				) {
					useConfigureStore.getState().setSettings({
						phoneDestination: message.phoneDestination,
						pcMusicDestination: message.pcMusicDestination,
					});
				}
				break;

			case "phone-destination":
				console.log(message);
				if (typeof message.path === "string") {
					useConfigureStore
						.getState()
						.setPhoneDestination(message.path);
				}
				break;

			case "pc-music-destination":
				console.log(message);
				if (typeof message.path === "string") {
					useConfigureStore
						.getState()
						.setPcMusicDestination(message.path);
				}
				break;
		}
	});
};

export const getSettings = () => sendToBackend({ type: "get-settings" });

export const openPcMusicDirectoryDialog = () =>
	sendToBackend({ type: "open-pc-music-directory-dialog" });

export const setPhoneDestination = (path: string) =>
	sendToBackend({ type: "set-phone-destination", path });

export const setPcMusicDestination = (path: string) =>
	sendToBackend({ type: "set-pc-music-destination", path });

export const getPhoneDestination = () =>
	sendToBackend({ type: "get-phone-destination" });

export const getPcMusicDestination = () =>
	sendToBackend({ type: "get-pc-music-destination" });
