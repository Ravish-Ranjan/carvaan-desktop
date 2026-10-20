import { useEffect } from "react";

import {
	getCombinedSize,
	initializeLibraryBridge,
	requestLibrary,
} from "@/lib/libraryBridge";

import { pages } from "@/components/pagetype";
import Sidebar from "@/components/Sidebar";
import { Library, Devices, Sync, Settings } from "@/components/sections";
import { getSettings, initializeConfigureBridge } from "./lib/configureBridge";
import Player from "./components/sections/Player";
import useNavigationStore from "./stores/navigationStore";

function App() {
	const { page, navigate } = useNavigationStore();

	const allCleanup = (cleanupFunctions: (() => void)[]) => {
		for (const cleanup of cleanupFunctions) {
			cleanup();
		}
	};

	useEffect(() => {
		const cleanupFuncs: (() => void)[] = [];
		
		cleanupFuncs.push(initializeLibraryBridge());
		cleanupFuncs.push(initializeConfigureBridge());

		requestLibrary();
		getCombinedSize();
		getSettings();

		return () => allCleanup(cleanupFuncs);
	}, []);

	return (
		<div className="flex h-screen w-full overflow-hidden bg-background text-foreground">
			{/* Sidebar */}
			<Sidebar page={page} setPage={navigate} />
			{/* Main content */}
			<main className="min-w-0 flex-1">
				<header className="flex h-16 items-center border-b px-6">
					<div>
						<h2 className="text-lg font-semibold">
							{pages.find((item) => item.id === page)?.label}
						</h2>
					</div>
				</header>
				<div className="h-[calc(100vh-4rem)] overflow-auto p-6">
					{page === "library" && <Library />}
					{page === "devices" && <Devices />}
					{page === "sync" && <Sync />}
					{page === "settings" && <Settings />}
					{page === "player" && <Player />}
				</div>
			</main>
		</div>
	);
}

export default App;
