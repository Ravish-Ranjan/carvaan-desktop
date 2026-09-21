import useLibraryStore from "@/stores/libraryStore";
import Button from "../ui/button";
import { openImportDialog } from "@/lib/libraryBridge";

function Library() {
	const { songs } = useLibraryStore();
	return (
		<section>
			<div className="flex justify-between">
				<div className="mb-6">
					<h3 className="text-2xl font-semibold">Your Library</h3>
					<p className="mt-1 text-sm text-muted-foreground">
						Manage your local Saregama music collection.
					</p>
				</div>
				<Button onClick={openImportDialog}>Import music files</Button>
			</div>

			<div className="grid gap-4 sm:grid-cols-3">
				<div className="rounded-xl border bg-card p-5">
					<p className="text-sm text-muted-foreground">Songs</p>
					<p className="mt-2 text-3xl font-semibold">
						{songs.length}
					</p>
				</div>

				<div className="rounded-xl border bg-card p-5">
					<p className="text-sm text-muted-foreground">On phone</p>
					<p className="mt-2 text-3xl font-semibold">0</p>
				</div>

				<div className="rounded-xl border bg-card p-5">
					<p className="text-sm text-muted-foreground">
						Pending sync
					</p>
					<p className="mt-2 text-3xl font-semibold">0</p>
				</div>
			</div>
			<ul>
				{songs.map((song) => {
					return <li key={song.id}>{song.name}</li>;
				})}
			</ul>
		</section>
	);
}

export default Library;
