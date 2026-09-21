import { useEffect, useState } from "react";

import { initializeLibraryBridge, requestLibrary } from "@/lib/libraryBridge";

import { pages, type Page } from "@/components/pagetype";
import Sidebar from "@/components/Sidebar";
import Library from "@/components/sections/Library";
import Devices from "@/components/sections/Devices";
import Settings from "@/components/sections/Settings";
import Sync from "@/components/sections/Sync";

function App() {
	const [page, setPage] = useState<Page>("library");

	useEffect(() => {
		const cleanup = initializeLibraryBridge();
		requestLibrary();

		return cleanup;
	}, []);

	return (
		<div className="flex h-screen w-full overflow-hidden bg-background text-foreground">
			{/* Sidebar */}
			<Sidebar page={page} setPage={setPage} />
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
				</div>
			</main>
		</div>
	);
}

export default App;
