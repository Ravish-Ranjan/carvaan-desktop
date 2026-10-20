import useLibraryStore from "@/stores/libraryStore";
import {
	Table,
	TableBody,
	TableCell,
	TableHead,
	TableHeader,
	TableRow,
} from "./ui/table";
import { sizeConverter } from "@/lib/songs";
import Badge from "./ui/badge";
import type { SongStatus } from "@/types/song";
import { clsx } from "cn";
import useNavigationStore from "@/stores/navigationStore";

const statusMap: Record<SongStatus, string> = {
	NEW: "bg-gray-500 dark:bg-gray-700",
	ERROR: "bg-red-500 dark:bg-red-700",
	MISSING_LOCAL: "bg-amber-500 dark:bg-amber-700",
	MISSING_PHONE: "bg-amber-500 dark:bg-amber-700",
	MODIFIED: "bg-sky-500 dark:bg-sky-700",
	PENDING_DELETE: "bg-amber-500 dark:bg-amber-700",
	SYNCED: "bg-emerald-500 dark:bg-emerald-700",
};

function SongsTable() {
	const { songs } = useLibraryStore();
	const { navigate } = useNavigationStore();
	return (
		<>
			<h2 className="text-xl font-semibold">Songs</h2>
			<Table>
				<TableHeader>
					<TableRow className="bg-secondary ">
						<TableHead className="font-mono text-muted-foreground">
							#id
						</TableHead>
						<TableHead>Title</TableHead>
						<TableHead>Size</TableHead>
						<TableHead>Status</TableHead>
					</TableRow>
				</TableHeader>
				<TableBody>
					{songs.map((song, i) => (
						<TableRow
							key={`song-${song.id}`}
							className={i % 2 == 1 ? "bg-card" : ""}
						>
							<TableCell className="font-mono">
								{song.id}
							</TableCell>
							<TableCell
								onClick={() => navigate("player", { song })}
								className="cursor-pointer"
							>
								{song.name}
							</TableCell>
							<TableCell className="font-mono">
								{sizeConverter(song.filesize)}
							</TableCell>
							<TableCell>
								<Badge
									variant={"default"}
									className={clsx(
										"font-semibold",
										statusMap[song.status],
									)}
								>
									{song.status}
								</Badge>
							</TableCell>
						</TableRow>
					))}
				</TableBody>
			</Table>
		</>
	);
}

export default SongsTable;
