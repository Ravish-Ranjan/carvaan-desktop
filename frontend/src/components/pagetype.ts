import { Library, RefreshCw, Settings, Smartphone } from "lucide-react";

export type Page = "library" | "devices" | "sync" | "settings";

export const pages: {
	id: Page;
	label: string;
	icon: typeof Library;
}[] = [
	{
		id: "library",
		label: "Library",
		icon: Library,
	},
	{
		id: "devices",
		label: "Devices",
		icon: Smartphone,
	},
	{
		id: "sync",
		label: "Sync",
		icon: RefreshCw,
	},
	{
		id: "settings",
		label: "Settings",
		icon: Settings,
	},
];
