import { ChevronUp, Pause, Play, Volume2, VolumeX } from "lucide-react";

import usePlayerStore from "@/stores/playerStore";
import useNavigationStore from "@/stores/navigationStore";
import { toggleAudioPlayback, setAudioMuted } from "@/lib/audioEngine";

const DEFAULT_COVER = "/assets/default-cover-art.jpeg";

interface MiniPlayerProps {
	collapsed: boolean;
}

function formatTime(seconds: number) {
	if (!Number.isFinite(seconds) || seconds < 0) return "0:00";

	const minutes = Math.floor(seconds / 60);
	const remainingSeconds = Math.floor(seconds % 60);

	return `${minutes}:${String(remainingSeconds).padStart(2, "0")}`;
}

function MiniPlayer({ collapsed }: MiniPlayerProps) {
	const {
		id,
		url,
		title,
		artist,
		coverArt,
		currentTime,
		duration,
		isPlaying,
		isMuted,
	} = usePlayerStore();

	const navigate = useNavigationStore((state) => state.navigate);

	const hasSong = Boolean(url);

	const progress = duration && duration > 0 ? Math.min((currentTime / duration) * 100, 100) : 0;

	const openPlayer = () => {
		if (!hasSong) {
			navigate("player");
			return;
		}

		navigate("player", {
			song: {
				id: id!,
				name: title,
				filepath: url!,
				filesize: 0,
				status: "SYNCED",
			},
		});
	};

	const handleMute = (event: React.MouseEvent) => {
		event.stopPropagation();
		setAudioMuted(!isMuted);
		usePlayerStore.getState().setIsMuted(!isMuted);
	};

	const handlePlayback = (event: React.MouseEvent) => {
		event.stopPropagation();
		void toggleAudioPlayback();
	};

	if (collapsed) {
		return (
			<div className="flex flex-col items-center gap-2 py-2">
				<button
					type="button"
					title={hasSong ? title : "No song selected"}
					onClick={openPlayer}
					className="group relative size-10 overflow-hidden rounded-lg bg-muted"
				>
					<img
						src={coverArt || DEFAULT_COVER}
						alt=""
						onError={(event) => {
							event.currentTarget.src = DEFAULT_COVER;
						}}
						className="size-full object-cover"
					/>

					{hasSong && (
						<span className="absolute inset-0 flex items-center justify-center bg-black/50 opacity-0 transition-opacity group-hover:opacity-100">
							<ChevronUp className="size-5 text-white" />
						</span>
					)}
				</button>

				<button
					type="button"
					title={isPlaying ? "Pause" : "Play"}
					disabled={!hasSong}
					onClick={handlePlayback}
					className="flex size-9 items-center justify-center rounded-lg transition-colors hover:bg-accent disabled:cursor-not-allowed disabled:opacity-40"
				>
					{isPlaying ? (
						<Pause className="size-4" fill="currentColor" />
					) : (
						<Play className="size-4" fill="currentColor" />
					)}
				</button>

				<div className="h-1 w-8 overflow-hidden rounded-full bg-muted">
					<div
						className="h-full bg-primary transition-[width]"
						style={{ width: `${progress}%` }}
					/>
				</div>
			</div>
		);
	}

	return (
		<section className="overflow-hidden rounded-xl border bg-card/70">
			{/* Track information */}
			<button
				type="button"
				onClick={openPlayer}
				className="flex w-full min-w-0 items-center gap-3 p-3 text-left transition-colors hover:bg-accent/50"
			>
				<div className="size-11 shrink-0 overflow-hidden rounded-lg bg-muted">
					<img
						src={coverArt || DEFAULT_COVER}
						alt=""
						onError={(event) => {
							event.currentTarget.src = DEFAULT_COVER;
						}}
						className="size-full object-cover"
					/>
				</div>

				<div className="min-w-0 flex-1">
					<p className="truncate text-sm font-medium">
						{hasSong ? title : "Nothing playing"}
					</p>

					<p className="truncate text-xs text-muted-foreground">
						{hasSong ? artist || "Unknown artist" : "Select a song"}
					</p>
				</div>

				<ChevronUp className="size-4 shrink-0 text-muted-foreground" />
			</button>

			{/* Playback progress */}
			<div className="h-1 bg-muted">
				<div
					className="h-full bg-primary transition-[width] duration-200"
					style={{ width: `${progress}%` }}
				/>
			</div>

			{/* Playback controls */}
			<div className="flex items-center justify-between px-3 py-2">
				<div className="flex items-center gap-1">
					<button
						type="button"
						title={isMuted ? "Unmute" : "Mute"}
						disabled={!hasSong}
						onClick={handleMute}
						className="flex size-8 items-center justify-center rounded-md text-muted-foreground transition-colors hover:bg-accent hover:text-foreground disabled:opacity-40"
					>
						{isMuted ? (
							<VolumeX className="size-4" />
						) : (
							<Volume2 className="size-4" />
						)}
					</button>

					<span className="text-[11px] tabular-nums text-muted-foreground">
						{formatTime(currentTime)}
						{" / "}
						{formatTime(duration ?? 0)}
					</span>
				</div>

				<button
					type="button"
					title={isPlaying ? "Pause" : "Play"}
					disabled={!hasSong}
					onClick={handlePlayback}
					className="flex size-9 items-center justify-center rounded-full bg-primary text-primary-foreground transition-opacity hover:opacity-85 disabled:cursor-not-allowed disabled:opacity-40"
				>
					{isPlaying ? (
						<Pause className="size-4" fill="currentColor" />
					) : (
						<Play className="size-4" fill="currentColor" />
					)}
				</button>
			</div>
		</section>
	);
}

export default MiniPlayer;
