import useLibraryStore from "@/stores/libraryStore";
import {
	Item,
	ItemContent,
	ItemDescription,
	ItemMedia,
	ItemTitle,
} from "../ui/item";
import { useEffect } from "react";
import useNavigationStore from "@/stores/navigationStore";
import MusicPlayer from "../MusicPlayer";
import { Avatar, AvatarFallback, AvatarImage } from "../ui/avatar";
import usePlayerStore from "@/stores/playerStore";
import { playerBridge } from "@/lib/playerBridge";

function Player() {
	const { songs } = useLibraryStore();
	const { currentSong } = usePlayerStore();
	const { params } = useNavigationStore();

	useEffect(() => {
		if (!params.song) return;

		const songId = params.song.id;

		useNavigationStore.getState().clearParams();

		playerBridge.playSong(songId);
	}, [params.song]);

	return (
		<section className="grid grid-cols-7  gap-2 h-full">
			<div className="col-span-4">
				<MusicPlayer song={currentSong} />
			</div>
			<div className="flex flex-col col-span-3 gap-1 outline overflow-y-auto rounded-md p-1">
				{songs.map((song) => (
					<Item
						key={song.id}
						variant={
							currentSong?.id === song.id ? "muted" : "outline"
						}
						onClick={() => playerBridge.playSong(song.id)}
						className="cursor-pointer"
					>
						<ItemMedia>
							<Avatar>
								<AvatarImage src={song.coverArt} />
								<AvatarFallback>
									{song.title.substring(0, 2)}
								</AvatarFallback>
							</Avatar>
						</ItemMedia>
						<ItemContent>
							<ItemTitle>{song.name}</ItemTitle>
							<ItemDescription>{song.artist}</ItemDescription>
						</ItemContent>
					</Item>
				))}
			</div>
		</section>
	);
}

export default Player;
