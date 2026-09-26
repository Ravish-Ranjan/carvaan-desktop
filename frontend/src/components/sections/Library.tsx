import useLibraryStore from "@/stores/libraryStore";
import Button from "../ui/button";
import { openImportDialog, openNewSongsInPicard } from "@/lib/libraryBridge";
import SongsTable from "../SongsTable";
import { Separator } from "../ui/separator";

function Library() {
	const { songs, combinedSize } = useLibraryStore();
	return (
		<section className="grid gap-4">
			<div className="flex justify-between">
				<div>
					<h3 className="text-2xl font-semibold">Your Library</h3>
					<p className="mt-1 text-sm text-muted-foreground">
						Manage your local Saregama music collection.
					</p>
				</div>
			</div>

			{/* stats */}
			<div className="grid gap-4 sm:grid-cols-4">
				<div className="rounded-xl border bg-card p-3">
					<p className="text-sm text-muted-foreground">Songs</p>
					<p className="mt-2 text-3xl font-semibold">
						{songs.length}
					</p>
				</div>

				<div className="rounded-xl border bg-card p-3">
					<p className="text-sm text-muted-foreground">On phone</p>
					<p className="mt-2 text-3xl font-semibold">
						{
							songs.filter((songs) => songs.status === "SYNCED")
								.length
						}
					</p>
				</div>

				<div className="rounded-xl border bg-card p-3">
					<p className="text-sm text-muted-foreground">
						Pending sync
					</p>
					<p className="mt-2 text-3xl font-semibold">
						{songs.filter((songs) => songs.status === "NEW").length}
					</p>
				</div>

				<div className="rounded-xl border bg-card p-3">
					<p className="text-sm text-muted-foreground">
						Combined size
					</p>
					<p className="mt-2 flex gap-1 items-end-safe">
						<span className="text-3xl font-semibold">
							{combinedSize.split(" ")[0]}
						</span>
						<span className="text-muted-foreground text-xl">
							{combinedSize.split(" ")[1]}
						</span>
					</p>
				</div>
			</div>

			<Separator />
			{/* operations */}
			<div className="text-lg grid sm:grid-cols-4 gap-2">
				<Button onClick={openNewSongsInPicard}>Open new songs in Picard</Button>
				<Button onClick={openImportDialog}>Import music files</Button>
			</div>

			<Separator />
			{/* songs table */}
			<SongsTable />
		</section>
	);
}

export default Library;
