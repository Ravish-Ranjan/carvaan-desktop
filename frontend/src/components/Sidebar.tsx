import { Music2, SidebarOpen } from "lucide-react";
import ThemeToggleButton from "./ThemeToggleButton";
import { pages, type Page } from "./pagetype";
import { useState } from "react";
import Button from "./ui/button";
import { clsx } from "cn";
import MiniPlayer from "./MiniPlayer";
import type { NavigationParams } from "@/stores/navigationStore";

function Logo({
	setOpenState,
}: {
	setOpenState: React.Dispatch<React.SetStateAction<boolean>>;
}) {
	const [hovering, setHovering] = useState(false);

	return (
		<Button
			variant={"default"}
			size={"icon-lg"}
			onClick={() => setOpenState((state) => !state)}
			onMouseEnter={() => setHovering(true)}
			onMouseLeave={() => setHovering(false)}
		>
			{hovering ? (
				<SidebarOpen className="size-5" />
			) : (
				<Music2 className="size-5" />
			)}
		</Button>
	);
}

function Sidebar({
	page,
	setPage,
}: {
	page: Page;
	setPage: (page: Page, params?: NavigationParams) => void;
}) {
	const [sidebarOpen, setSidebarOpen] = useState(true);

	return (
		<aside
			className={clsx(
				"flex shrink-0 flex-col border-r bg-muted/20",
				sidebarOpen ? "w-60" : "w-17",
			)}
		>
			{/* App header */}
			<div className="flex h-16 items-center gap-3 border-b px-4">
				<Logo setOpenState={setSidebarOpen} />

				{sidebarOpen && (
					<div>
						<h1 className="text-sm font-semibold">Sync Songs</h1>
						<p className="text-xs text-muted-foreground">
							Music manager
						</p>
					</div>
				)}
			</div>

			{/* Navigation */}
			<nav
				className={clsx(
					"flex-1 space-y-1 py-3",
					sidebarOpen ? "px-3" : "px-3",
				)}
			>
				{pages.map((item) => {
					const Icon = item.icon;
					const active = page === item.id;

					return (
						<Button
							key={item.id}
							title={item.label}
							onClick={() => setPage(item.id)}
							variant={active ? "default" : "outline"}
							className={clsx(
								"flex w-full h-10 items-center gap-2.5 rounded-lg px-3 py-2.5",
								"text-sm transition-colors",
								!sidebarOpen
									? "justify-center"
									: "justify-start",
							)}
						>
							<Icon className="size-4" />
							{sidebarOpen && <span>{item.label}</span>}
						</Button>
					);
				})}
			</nav>

			{/* Global mini-player */}
			<div
				className={clsx("border-t py-3", sidebarOpen ? "px-3" : "px-2")}
			>
				<MiniPlayer collapsed={!sidebarOpen} />
			</div>

			{/* Status */}
			<div className="border-t p-4 flex justify-between">
				{sidebarOpen && (
					<div className="flex items-center gap-2 text-xs text-muted-foreground">
						<span className="size-2 rounded-full bg-green-500" />
						<span>C++ backend ready</span>
					</div>
				)}
				<ThemeToggleButton />
			</div>
		</aside>
	);
}

export default Sidebar;
