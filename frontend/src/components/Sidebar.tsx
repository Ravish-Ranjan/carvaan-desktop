import { Music2 } from "lucide-react";
import ThemeToggleButton from "./ThemeToggleButton";
import { pages, type Page } from "./pagetype";

function Sidebar({
	page,
	setPage,
}: {
	page: Page;
	setPage: React.Dispatch<React.SetStateAction<Page>>;
}) {
	return (
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
			<div className="border-t p-4 flex justify-between">
				<div className="flex items-center gap-2 text-xs text-muted-foreground">
					<span className="size-2 rounded-full bg-green-500" />
					<span>C++ backend ready</span>
				</div>
				<ThemeToggleButton />
			</div>
		</aside>
	);
}

export default Sidebar;
