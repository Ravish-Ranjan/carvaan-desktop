import { useEffect, useState } from "react";
import { Library, Smartphone, RefreshCw, Settings, Music2 } from "lucide-react";

type Page = "library" | "devices" | "sync" | "settings";

const pages: {
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

function App() {
	const [page, setPage] = useState<Page>("library");
	const [songsCount, setSongsCount] = useState<number>(0);

	useEffect(() => {
		console.log("React Sync Songs frontend loaded");
		if (window.webkit?.messageHandlers?.syncSongs) {
			window.webkit.messageHandlers.syncSongs.postMessage(
				JSON.stringify({ type: "frontend-ready" }),
			);
		}

		const handleCppMessage = (event: Event) => {
			const customEvent = event as CustomEvent;
			console.log("C++ -> React:", JSON.stringify(customEvent.detail));
			const message = customEvent.detail;
			
			if (message?.type === "library-import-result") {
				console.log(`Imported ${message.imported} song(s)`);
				setSongsCount(message?.imported)
			}
			
		};

		window.addEventListener("syncsongs-message", handleCppMessage);

		return () => {
			window.removeEventListener("syncsongs-message", handleCppMessage);
		};
	}, []);

	return (
		<div className="flex h-screen w-full overflow-hidden bg-background text-foreground">
			{/* Sidebar */}
			<aside className="flex w-60 shrink-0 flex-col border-r bg-muted/20">
				{/* App header */}
				<div className="flex h-16 items-center gap-3 border-b px-5">
					<div className="flex size-9 items-center justify-center rounded-lg bg-primary text-primary-foreground">
						<Music2 className="size-5" />
					</div>

					<div>
						<h1 className="text-sm font-semibold">Sync Songs</h1>
						<p className="text-xs text-muted-foreground">
							Music manager
						</p>
					</div>
				</div>

				{/* Navigation */}
				<nav className="flex-1 space-y-1 p-3">
					{pages.map((item) => {
						const Icon = item.icon;
						const active = page === item.id;

						return (
							<button
								key={item.id}
								type="button"
								onClick={() => setPage(item.id)}
								className={[
									"flex w-full items-center gap-3 rounded-lg px-3 py-2.5",
									"text-sm transition-colors",
									active
										? "bg-primary text-primary-foreground"
										: "text-muted-foreground hover:bg-muted hover:text-foreground",
								].join(" ")}
							>
								<Icon className="size-4" />
								<span>{item.label}</span>
							</button>
						);
					})}
				</nav>

				{/* Status */}
				<div className="border-t p-4">
					<div className="flex items-center gap-2 text-xs text-muted-foreground">
						<span className="size-2 rounded-full bg-green-500" />
						<span>C++ backend ready</span>
					</div>
				</div>
			</aside>

			{/* Main content */}
			<main className="min-w-0 flex-1">
				<header className="flex h-16 items-center border-b px-6">
					<div>
						<h2 className="text-lg font-semibold">
							{pages.find((item) => item.id === page)?.label}
						</h2>
					</div>
				</header>
				<button
					onClick={() => {
						window.webkit?.messageHandlers?.syncSongs?.postMessage(
							JSON.stringify({
								type: "library-import-files",
								files: [
									"/home/ravish/Music/saregama carvaan/Songs/Paisa O Paisa.mp3",
									"/tmp/test-song.flac",
								],
							}),
						);
					}}
				>
					Test Library Import
				</button>
				<button
					onClick={() => {
						window.webkit?.messageHandlers?.syncSongs?.postMessage(
							JSON.stringify({
								type: "open-import-dialog",
							}),
						);
					}}
				>
					Import Songs
				</button>
				<div className="h-[calc(100vh-4rem)] overflow-auto p-6">
					{page === "library" && (
						<section>
							<div className="mb-6">
								<h3 className="text-2xl font-semibold">
									Your Library
								</h3>

								<p className="mt-1 text-sm text-muted-foreground">
									Manage your local Saregama music collection.
								</p>
							</div>

							<div className="grid gap-4 sm:grid-cols-3">
								<div className="rounded-xl border bg-card p-5">
									<p className="text-sm text-muted-foreground">
										Songs
									</p>
									<p className="mt-2 text-3xl font-semibold">
										{songsCount}
									</p>
								</div>

								<div className="rounded-xl border bg-card p-5">
									<p className="text-sm text-muted-foreground">
										On phone
									</p>
									<p className="mt-2 text-3xl font-semibold">
										0
									</p>
								</div>

								<div className="rounded-xl border bg-card p-5">
									<p className="text-sm text-muted-foreground">
										Pending sync
									</p>
									<p className="mt-2 text-3xl font-semibold">
										0
									</p>
								</div>
							</div>
						</section>
					)}

					{page === "devices" && (
						<section>
							<h3 className="text-2xl font-semibold">
								ADB Devices
							</h3>

							<p className="mt-1 text-sm text-muted-foreground">
								Connected Android devices will appear here.
							</p>
						</section>
					)}

					{page === "sync" && (
						<section>
							<h3 className="text-2xl font-semibold">
								Synchronization
							</h3>

							<p className="mt-1 text-sm text-muted-foreground">
								Review and synchronize changes between your
								library and phone.
							</p>
						</section>
					)}

					{page === "settings" && (
						<section>
							<h3 className="text-2xl font-semibold">Settings</h3>

							<p className="mt-1 text-sm text-muted-foreground">
								Configure your library, phone destination and
								synchronization behavior.
							</p>
						</section>
					)}
				</div>
			</main>
		</div>
	);
}

export default App;
