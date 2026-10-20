import { useState } from "react";
import {
	CircleAlert,
	// Heart,
	// ListMusic,
	Pause,
	Play,
	Repeat,
	Repeat1,
	RepeatOff,
	RotateCcw,
	Shuffle,
	SkipBack,
	SkipForward,
	Volume2,
	VolumeX,
} from "lucide-react";
import type { Song } from "@/types/song";
import usePlayerStore from "@/stores/playerStore";
import { playerBridge } from "@/lib/playerBridge";

const DEFAULT_COVER = "/assets/default-cover-art.jpeg";

function formatTime(seconds: number): string {
	if (!Number.isFinite(seconds) || seconds < 0) return "0:00";

	const minutes = Math.floor(seconds / 60);
	const remainingSeconds = Math.floor(seconds % 60);

	return `${minutes}:${String(remainingSeconds).padStart(2, "0")}`;
}

function MusicPlayer({ song }: { song: Song | null }) {
	// const [isPlaying, setIsPlaying] = useState(false);
	// const [isLiked, setIsLiked] = useState(false);
	// const [isMuted, setIsMuted] = useState(false);
	// const [isShuffle, setIsShuffle] = useState(false);
	// const [isRepeat, setIsRepeat] = useState(false);
	const [volume, setVolume] = useState(70);

	const { muted, shuffle, repeat, position, playstate } = usePlayerStore();
	const {
		setMuted,
		setShuffle,
		setRepeat,
		seek,
		pause,
		resume,
		next,
		previous,
		playSong,
	} = playerBridge;

	const duration = song?.duration || 0;
	const progress = duration > 0 ? (position / duration) * 100 : 0;
	const title = song?.title || song?.name;
	const artist = song?.artist || "Unknown artist";
	const album = song?.album || "Unknown album";

	const handleMuted = (status: boolean | undefined) => {
		if (status) {
			setMuted(status);
		} else {
			setMuted(!muted);
		}
	};

	const handleShuffle = () => {
		setShuffle(!shuffle);
	};

	const handleRepeat = () => {
		if (repeat === "all") setRepeat("one");
		if (repeat === "one") setRepeat("off");
		if (repeat === "off") setRepeat("all");
	};

	const handlePosition = (value: number) => {
		seek(value);
	};

	const handlePause = pause;

	const handleResume = resume;

	const handleNext = next;

	const handlePrevious = previous;

	const handleEnded = () => {
		next();
	};

	const handleStopped = () => {
		if (song) playSong(song?.id);
		else next();
	};

	const handleError = next;

	return (
		<section
			className={
				"relative flex h-full min-h-0 flex-col overflow-hidden rounded-2xl dark:bg-[#111113] bg-[#11111300]"
			}
		>
			{/* Ambient background */}
			<div className="pointer-events-none absolute inset-0 overflow-hidden">
				<div className="absolute -top-24 left-1/4 h-96 w-96 rounded-full bg-emerald-600/15 blur-[120px]" />
				<div className="absolute bottom-0 right-0 h-80 w-80 rounded-full bg-green-500/10 blur-[110px]" />
				<div className="absolute inset-0 bg-linear-to-b from-white/[0.035] via-transparent dark:to-black/50 to-white/50" />
			</div>

			{/* Main content: vertical player layout */}
			<div className="relative z-10 min-h-0 grid grid-cols-5 grid-rows-2 items-center gap-x-4 gap-y-8 overflow-y-auto px-5 py-5 sm:px-4">
				{/* 1. Album artwork */}
				<div className="w-full max-w-[min(72vw,360px)] shrink-0 col-span-2 row-span-1">
					<div className="aspect-square overflow-hidden rounded-xl bg-white/6 shadow-2xl shadow-black/50 ring-1 ring-white/10">
						<img
							src={song?.coverArt || DEFAULT_COVER}
							alt={`${title} album artwork`}
							onError={(event) => {
								if (
									event.currentTarget.src.endsWith(
										DEFAULT_COVER,
									)
								)
									return;
								event.currentTarget.src = DEFAULT_COVER;
							}}
							className="h-full w-full object-cover"
						/>
					</div>
				</div>

				{/* 2. Track information */}
				<div className="w-full max-w-md dark:text-white col-span-3 row-span-1">
					<h2 className="line-clamp-1 text-lg font-bold tracking-tight sm:text-md">
						{song ? title : ""}
					</h2>

					<p className="md:mt-1 truncate text-sm dark:text-white/60">
						{song ? artist : "Select a track to start playing"}
						{song ? song?.year && <> | {song?.year}</> : ""}
					</p>
					<p className="md:mt-1 truncate text-sm dark:text-white/35">
						{song ? album : ""}
					</p>
				</div>

				{/* 3. Playback controls */}
				<div className="w-full col-span-5">
					<div>
						<input
							type="range"
							min={0}
							max={Math.max(duration, 1)}
							step={1}
							value={Math.min(position, duration || 0)}
							disabled={!song}
							onChange={(event) =>
								handlePosition(Number(event.target.value))
							}
							aria-label="Playback position"
							style={{
								background: `linear-gradient(to right, oklch(76.5% 0.177 163.223) ${progress}%, rgba(127,127,127,0.50) ${progress}%)`,
							}}
							className="h-1.5 w-full cursor-pointer appearance-none rounded-full accent-emerald-400 disabled:cursor-not-allowed"
						/>
						<div className="mt-1 flex justify-between text-xs tabular-nums dark:text-white/40">
							<span>{formatTime(position)}</span>
							<span>{formatTime(duration)}</span>
						</div>
					</div>

					<div className="mt-1 flex items-center justify-between">
						<button
							type="button"
							title="Shuffle"
							aria-pressed={shuffle}
							disabled={!song}
							onClick={() => handleShuffle()}
							className={`rounded-full p-2 transition hover:bg-white/10 disabled:cursor-not-allowed ${
								shuffle
									? "text-emerald-400"
									: "dark:text-white/45 hover:dark:text-white"
							}`}
						>
							<Shuffle className="size-3 md:size-4.75" />
						</button>

						<button
							type="button"
							disabled={!song}
							title="Previous track"
							onClick={() => handlePrevious()}
							className="rounded-full p-2 dark:text-white/80 transition hover:scale-105 hover:dark:text-white disabled:cursor-not-allowed"
						>
							<SkipBack
								className="size-3 md:size-6"
								fill="currentColor"
							/>
						</button>

						<button
							type="button"
							title={playstate}
							disabled={!song}
							onClick={() => {
								switch (playstate) {
									case "playing":
										handlePause();
										break;
									case "paused":
										handleResume();
										break;
									case "ended":
										handleEnded();
										break;
									case "stopped":
										handleStopped();
										break;
									case "error":
										handleError();
										break;
									default:
								}
							}}
							className="flex size-9 md:size-16 items-center justify-center rounded-full bg-white text-black shadow-lg transition hover:scale-105 hover:bg-white/90 active:scale-95 disabled:cursor-not-allowed"
						>
							{playstate === "playing" && (
								<Pause
									className="size-6 md:size-9"
									fill="currentColor"
								/>
							)}
							{playstate === "paused" && (
								<Play
									className="size-6 md:size-9"
									fill="currentColor"
								/>
							)}
							{playstate === "ended" && (
								<RotateCcw
									className="size-6 md:size-9"
									fill="currentColor"
								/>
							)}
							{playstate === "error" && (
								<CircleAlert
									className="size-6 md:size-9"
									fill="currentColor"
								/>
							)}
							{playstate === "stopped" && (
								<Play
									className="size-6 md:size-9"
									fill="currentColor"
								/>
							)}
						</button>

						<button
							type="button"
							disabled={!song}
							title="Next track"
							onClick={() => handleNext()}
							className="rounded-full p-2 dark:text-white/80 transition hover:scale-105 hover:dark:text-white disabled:cursor-not-allowed"
						>
							<SkipForward
								className="size-3 md:size-6"
								fill="currentColor"
							/>
						</button>

						<button
							type="button"
							title="Repeat"
							disabled={!song}
							aria-pressed={repeat == "all" || repeat == "one"}
							onClick={() => handleRepeat()}
							className={`rounded-full p-2 transition hover:bg-white/10 disabled:cursor-not-allowed ${
								repeat == "all" || repeat == "one"
									? "text-emerald-400"
									: "dark:text-white/45 hover:dark:text-white"
							}`}
						>
							{repeat === "all" && <Repeat />}
							{repeat === "off" && <RepeatOff />}
							{repeat === "one" && <Repeat1 />}
						</button>
					</div>

					{/* Volume and secondary controls */}
					<div className="mt-1 flex items-center justify-center border-t border-white/8 ">
						{/* <button
							type="button"
							disabled={!song}
							title={isLiked ? "Unlike song" : "Like song"}
							aria-pressed={isLiked}
							onClick={() => setIsLiked(!isLiked)}
							className={`rounded-full p-2 transition hover:bg-white/10 ${
								isLiked
									? "text-emerald-400"
									: "dark:text-white/45 hover:dark:text-white disabled:cursor-not-allowed"
							}`}
						>
							<Heart
								size={20}
								fill={isLiked ? "currentColor" : "none"}
							/>
						</button> */}

						<div className="flex items-center gap-3">
							<button
								type="button"
								title={muted ? "Unmute" : "Mute"}
								disabled={!song}
								onClick={() => handleMuted(undefined)}
								className="dark:text-white/50 transition hover:dark:text-white disabled:cursor-not-allowed"
							>
								{muted || volume === 0 ? (
									<VolumeX size={19} />
								) : (
									<Volume2 size={19} />
								)}
							</button>
							<input
								type="range"
								min={0}
								max={100}
								disabled={!song}
								value={muted ? 0 : volume}
								onChange={(event) => {
									setVolume(Number(event.target.value));
									handleMuted(false);
								}}
								aria-label="Volume"
								className="h-1 w-48 cursor-pointer accent-emerald-400 disabled:cursor-not-allowed"
							/>
						</div>

						{/* <button
							type="button"
							disabled={!song}
							title="Queue"
							className="rounded-full p-2 dark:text-white/45 transition hover:bg-white/10 hover:dark:text-white disabled:cursor-not-allowed"
						>
							<ListMusic size={20} />
						</button> */}
					</div>
				</div>
			</div>
		</section>
	);
}

export default MusicPlayer;
