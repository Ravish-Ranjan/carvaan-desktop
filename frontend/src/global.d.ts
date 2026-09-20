declare global {
	interface Window {
		webkit?: {
			messageHandlers?: {
				syncSongs?: {
					postMessage: (mesage: string) => void;
				};
			};
		};
	}
}

export {};
