import { create } from "zustand";
import type { Song } from "@/types/song";
import type { Page } from "@/components/pagetype";

export type NavigationParams = {
	song?: Song;
};
interface NavigationStore {
	page: Page;
	params: NavigationParams;

	navigate: (page: Page, params?: NavigationParams) => void;
	clearParams: () => void;
}

const useNavigationStore = create<NavigationStore>((set) => ({
	page: "library",
	params: {},

	navigate: (page, params = {}) => set({ page, params }),

	clearParams: () => set({ params: {} }),
}));

export default useNavigationStore;
