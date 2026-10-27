import { useEffect, useRef, useState } from "react";
import useLibraryStore from "@/stores/libraryStore";
import { Item, ItemContent, ItemMedia, ItemTitle } from "../ui/item";
import useNavigationStore from "@/stores/navigationStore";
import MusicPlayer from "../MusicPlayer";
import usePlayerStore from "@/stores/playerStore";
import { Avatar, AvatarFallback } from "../ui/avatar";
import { playLibrarySong } from "@/lib/queue";
import { Input } from "../ui/input";
import type { Song } from "@/types/song";

function Player() {
	const songs = useLibraryStore((state) => state.songs);
	const currentSongId = usePlayerStore((state) => state.id);
	const params = useNavigationStore((state) => state.params);
	const lastHandledSongId = useRef<number | null>(null);
	const [filtered, setFiltered] = useState<Song[]>([]);
	const [debouncedQuery, setDebouncedQuery] = useState("");
	const [query, setQuery] = useState("");

	useEffect(() => {
		const requestedSong = params.song;

		if (!requestedSong) {
			lastHandledSongId.current = null;
			return;
		}

		const songId = requestedSong.id;

		if (lastHandledSongId.current === songId) return;

		const songExists = songs.some((song) => song.id === songId);
		if (!songExists) return;

		lastHandledSongId.current = songId;

		useNavigationStore.getState().clearParams();

		playLibrarySong(songs, songId);
	}, [params.song, songs]);

	useEffect(() => {
		const timer = setTimeout(() => {
			setDebouncedQuery(query);
		}, 500);

		return () => clearTimeout(timer);
	}, [query]);

	useEffect(() => {
		(async () => {
			if (debouncedQuery.trim() === "") {
				console.log("Clear results or do nothing");
				return;
			}
			const searchquery = debouncedQuery.toLocaleLowerCase();
			const filtered = songs.filter(
				(song) =>
					song.name.toLocaleLowerCase().includes(searchquery) ||
					searchquery.includes(song.name.toLocaleLowerCase()),
			);
			setFiltered(filtered);
		})();
	}, [debouncedQuery, songs]);

	return (
		<section className="grid h-full min-h-0 grid-cols-7 gap-2 overflow-hidden">
			{/* Music Player */}
			<div className="col-span-4 min-h-0 min-w-0 overflow-auto">
				<MusicPlayer />
			</div>

			{/* Song List */}
			<div className="col-span-3 flex min-h-0 min-w-0 flex-col gap-2 overflow-hidden rounded-md p-1">
				{/* Search */}
				<div className="shrink-0">
					<Input
						type="search"
						placeholder="Search Song"
						value={query}
						className="shadow-md shadow-secondary-foreground/30"
						onChange={(e) => setQuery(e.target.value)}
					/>
				</div>

				{/* Scrollable List */}
				<div className="flex min-h-0 flex-1 flex-col gap-1 overflow-y-auto overscroll-contain">
					{(debouncedQuery.trim() === "" ? songs : filtered).map(
						(song) => (
							<Item
								key={song.id}
								variant={
									currentSongId === song.id
										? "muted"
										: "default"
								}
								id={`player-list-id-${song.id}`}
								onClick={() => {
									lastHandledSongId.current = song.id;
									playLibrarySong(songs, song.id);
									setQuery("");
								}}
								className="shrink-0 cursor-pointer"
							>
								<ItemMedia>
									<Avatar>
										<AvatarFallback>
											{song.name.substring(0, 2)}
										</AvatarFallback>
									</Avatar>
								</ItemMedia>

								<ItemContent>
									<ItemTitle>{song.name}</ItemTitle>
								</ItemContent>
							</Item>
						),
					)}
				</div>
			</div>
		</section>
	);
}

export default Player;
