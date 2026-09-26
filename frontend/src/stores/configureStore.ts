import { create } from "zustand";

interface ConfigureState {
	phoneDestination: string;
	pcMusicDestination: string;

	setPhoneDestination: (path: string) => void;
	setPcMusicDestination: (path: string) => void;

	setSettings: (settings: {
		phoneDestination: string;
		pcMusicDestination: string;
	}) => void;

	reset: () => void;
}

const initialState = { phoneDestination: "", pcMusicDestination: "" };

export const useConfigureStore = create<ConfigureState>((set) => ({
	...initialState,

	setPhoneDestination: (path) => set({ phoneDestination: path }),

	setPcMusicDestination: (path) => set({ pcMusicDestination: path }),

	setSettings: (settings) =>
		set({
			phoneDestination: settings.phoneDestination,
			pcMusicDestination: settings.pcMusicDestination,
		}),

	reset: () => set({ ...initialState }),
}));
