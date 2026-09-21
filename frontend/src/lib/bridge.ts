import type { BackendMessage, FrontendMessage } from "@/types/protocol";

type BackendMessageHandler = (message: BackendMessage) => void;

export const sendToBackend = (message: FrontendMessage): void => {
	const handler = window.webkit?.messageHandlers?.syncSongs;

	if (!handler) {
		console.warn("[Bridge] syncSongs message handler is unavailable");
		return;
	}

	const serialized = JSON.stringify(message);
	console.log("[React → C++]", serialized);
	handler.postMessage(serialized);
};

export const subscribeToBackend = (
	handler: BackendMessageHandler,
): (() => void) => {
	const handleMessage = (event: Event) => {
		const customEvent = event as CustomEvent<unknown>;

		const message = customEvent.detail;

		console.log("[C++ → React]", message);

		if (!isBackendMessage(message)) {
			console.warn("[Bridge] Invalid backend message:", message);
			return;
		}

		handler(message);
	};

	window.addEventListener("syncsongs-message", handleMessage);

	return () => window.removeEventListener("syncsongs-message", handleMessage);
};

const isBackendMessage = (message: unknown): message is BackendMessage => {
	if (
		typeof message !== "object" ||
		message === null ||
		!("type" in message)
	) {
		return false;
	}

	const type = (message as { type: unknown }).type;

	switch (type) {
		case "library-songs":
		case "library-import-result":
			return true;

		default:
			return false;
	}
};
