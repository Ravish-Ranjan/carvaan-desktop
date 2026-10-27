import {
	Heart,
	Music2,
	Pause,
	Play,
	Repeat,
	Repeat1,
	Shuffle,
	SkipBack,
	SkipForward,
	Volume2,
	VolumeX,
} from "lucide-react";
import { useEffect, useState } from "react";

import usePlayerStore from "@/stores/playerStore";
import {
	toggleAudioPlayback,
	seekAudio,
	setAudioMuted,
	setAudioVolume,
} from "@/lib/audioEngine";
import { playNextTrack, playPreviousTrack } from "@/lib/playerControls";
import { getMajorColors } from "@/lib/coverArt";

const DEFAULT_COVER = "/assets/default-cover-art.jpeg";

function formatTime(seconds: number) {
	if (!Number.isFinite(seconds) || seconds < 0) return "0:00";

	const minutes = Math.floor(seconds / 60);
	const remainingSeconds = Math.floor(seconds % 60);

	return `${minutes}:${String(remainingSeconds).padStart(2, "0")}`;
}

function MusicPlayer() {
	const {
		url,
		title,
		artist,
		album,
		year,
		coverArt,
		duration,
		currentTime,
		isPlaying,
		volume,
		isMuted,
		shuffle,
		repeat,
		setIsMuted,
		setVolume,
		setShuffle,
		setRepeat,
	} = usePlayerStore();

	const [liked, setLiked] = useState(false);
	const [artworkError, setArtworkError] = useState(false);
	const [showRemaining, setShowRemaining] = useState(false);
	const [accentColor, setAccentColor] = useState<{
		primary: string;
		secondary: string;
		tertiary: string;
	}>({ primary: "17 17 19", secondary: "17 17 19", tertiary: "17 17 19" });

	const hasSong = Boolean(url);

	const progress =
		duration && duration > 0
			? Math.min((currentTime / duration) * 100, 100)
			: 0;

	const displayedTime = showRemaining
		? `-${formatTime(Math.max(0, (duration ?? 0) - currentTime))}`
		: formatTime(currentTime);

	const handleSeek = (value: number) => {
		if (!hasSong || !Number.isFinite(duration) || duration! <= 0) return;

		seekAudio(value);
	};

	const handleVolumeChange = (value: number) => {
		const nextVolume = Math.max(0, Math.min(1, value));

		setVolume(nextVolume);
		setAudioVolume(nextVolume);

		if (nextVolume > 0) {
			setIsMuted(false);
			setAudioMuted(false);
		}
	};

	const handleMute = () => {
		const nextMuted = !isMuted;

		setIsMuted(nextMuted);
		setAudioMuted(nextMuted);
	};

	const handleRepeat = () => {
		setRepeat(repeat === "off" ? "all" : repeat === "all" ? "one" : "off");
	};

	useEffect(() => {
		(async function () {
			const colour = await getMajorColors(coverArt, 3);
			setAccentColor({
				primary: colour[0],
				secondary: colour[1],
				tertiary: colour[2],
			});
			console.log(colour);
		})();
	}, [coverArt]);

	return (
		<div
			className={`relative flex aspect-square w-full h-full min-h-0 flex-col overflow-hidden rounded-2xl bg-transparent`}
		>
			{/* Ambient background */}
			<div className="pointer-events-none absolute inset-0 overflow-hidden">
				{/* Top accent glow */}
				<div
					className="absolute -top-24 left-1/4 size-96 rounded-full blur-[120px]"
					style={{
						backgroundColor: `rgb(${accentColor.tertiary} / 0.28)`,
					}}
				/>

				{/* Bottom-right accent glow */}
				<div
					className="absolute -bottom-20 -right-20 size-80 rounded-full blur-[110px]"
					style={{
						backgroundColor: `rgb(${accentColor.primary} / 0.35)`,
					}}
				/>

				{/* Subtle fade at the bottom; no solid background fill */}
				<div
					className="absolute inset-0"
					style={{
						backgroundImage: `linear-gradient(
							to bottom,
							transparent 35%,
							rgb(${accentColor.secondary} / 0.08) 65%,
							transparent 100%
						)`,
					}}
				/>
			</div>

			{/* Header */}
			<header className="relative z-10 flex shrink-0 items-center justify-between border-b border-white/8 px-4 py-3">
				<div>
					<p className="text-[10px] font-medium uppercase tracking-[0.2em] text-foreground/40">
						Sync Songs
					</p>
					<h1 className="mt-1 text-sm font-semibold">Now Playing</h1>
				</div>

				<div className="flex size-8 items-center justify-center rounded-full border border-white/10 bg-white/5">
					<Music2
						className="size-4"
						style={{ color: `rgb(${accentColor.primary})` }}
					/>
				</div>
			</header>
			{/* Main player */}
			<div className="relative z-10 flex min-h-0 flex-1 flex-col justify-evenly gap-1 overflow-hidden p-4">
				{/* Album artwork + Track information */}
				<div className="flex min-h-0 items-center gap-4">
					{/* Album artwork */}
					<div className="w-[42%] shrink-0">
						<div
							className="relative aspect-square overflow-hidden rounded-xl bg-white/5 ring-1 ring-white/10"
							style={{
								boxShadow: `0px 20px 25px -5px rgba(${accentColor.secondary} / 0.5)`,
							}}
						>
							<img
								src={
									artworkError
										? DEFAULT_COVER
										: coverArt || DEFAULT_COVER
								}
								alt={
									hasSong
										? `${title} album artwork`
										: "Album artwork"
								}
								onError={() => setArtworkError(true)}
								className="size-full object-cover"
							/>

							{!hasSong && (
								<div className="absolute inset-0 flex items-center justify-center bg-secondary-forground/20 text-foreground/50">
									<Music2 className="size-10" />
								</div>
							)}
						</div>
					</div>

					{/* Track information */}
					<div className="flex min-w-0 flex-1 items-center gap-2">
						<div className="min-w-0 flex-1">
							<h2 className="truncate text-base font-bold tracking-tight">
								{hasSong
									? title || "Unknown title"
									: "Nothing playing"}
							</h2>

							<p className="mt-1 truncate text-sm text-foreground/55">
								{hasSong
									? artist || "Unknown artist"
									: "Choose a song from your library"}
								{hasSong && year ? ` · ${year}` : ""}
							</p>

							{hasSong && album && (
								<p className="mt-1 truncate text-xs text-foreground/35">
									{album}
								</p>
							)}
						</div>

						<button
							type="button"
							title={liked ? "Unlike track" : "Like track"}
							aria-pressed={liked}
							disabled={!hasSong}
							onClick={() => setLiked((value) => !value)}
							className="flex size-8 shrink-0 items-center justify-center rounded-full transition hover:bg-white/10 disabled:opacity-30"
						>
							<Heart
								className={`size-4 ${
									liked
										? "fill-emerald-400 text-emerald-400"
										: "text-foreground/55"
								}`}
							/>
						</button>
					</div>
				</div>

				{/* Playback controls */}
				<div className="w-full">
					{/* Progress */}
					<div className="flex items-center gap-2">
						<span className="w-9 shrink-0 text-right text-[10px] tabular-nums text-foreground/45">
							{displayedTime}
						</span>

						<input
							type="range"
							min={0}
							max={duration && duration > 0 ? duration : 0}
							step={0.1}
							value={Math.min(currentTime, duration ?? 0)}
							disabled={!hasSong || !duration}
							onChange={(event) =>
								handleSeek(Number(event.target.value))
							}
							aria-label="Playback position"
							style={{
								background: `linear-gradient(to right, #14a379 ${progress}%, rgba(255,255,255,0.14) ${progress}%)`,
							}}
							className="h-1.5 min-w-0 flex-1 cursor-pointer appearance-none rounded-full disabled:cursor-not-allowed disabled:opacity-40 [&::-webkit-slider-thumb]:size-3 [&::-webkit-slider-thumb]:appearance-none [&::-webkit-slider-thumb]:rounded-full [&::-webkit-slider-thumb]:bg-white [&::-webkit-slider-thumb]:shadow"
						/>

						<button
							type="button"
							title="Toggle remaining time"
							onClick={() => setShowRemaining((value) => !value)}
							className="w-9 shrink-0 text-left text-[10px] tabular-nums text-foreground/45 hover:text-foreground"
						>
							{formatTime(duration ?? 0)}
						</button>
					</div>

					{/* Playback buttons */}
					<div className="mt-3 flex items-center justify-between">
						<button
							type="button"
							title="Shuffle"
							aria-pressed={shuffle}
							disabled={!hasSong}
							onClick={() => setShuffle(!shuffle)}
							className={`flex size-8 items-center justify-center rounded-full transition hover:bg-white/10 disabled:opacity-30 ${
								shuffle
									? "text-emerald-400"
									: "text-foreground/45"
							}`}
						>
							<Shuffle className="size-4" />
						</button>

						<button
							type="button"
							title="Previous track"
							disabled={!hasSong}
							onClick={playPreviousTrack}
							className="flex size-8 items-center justify-center rounded-full text-foreground/80 transition hover:bg-white/10 disabled:opacity-30"
						>
							<SkipBack className="size-4" fill="currentColor" />
						</button>

						<button
							type="button"
							title={isPlaying ? "Pause" : "Play"}
							disabled={!hasSong}
							onClick={() => void toggleAudioPlayback()}
							className="flex size-12 items-center justify-center rounded-full bg-white text-black shadow-xl shadow-black/30 transition hover:bg-white/90 active:scale-95 disabled:cursor-not-allowed disabled:opacity-40"
						>
							{isPlaying ? (
								<Pause className="size-5" fill="currentColor" />
							) : (
								<Play
									className="ml-1 size-5"
									fill="currentColor"
								/>
							)}
						</button>

						<button
							type="button"
							title="Next track"
							disabled={!hasSong}
							onClick={playNextTrack}
							className="flex size-8 items-center justify-center rounded-full text-foreground/80 transition hover:bg-white/10 disabled:opacity-30"
						>
							<SkipForward
								className="size-4"
								fill="currentColor"
							/>
						</button>

						<button
							type="button"
							title={`Repeat: ${repeat}`}
							aria-pressed={repeat !== "off"}
							disabled={!hasSong}
							onClick={handleRepeat}
							className={`flex size-8 items-center justify-center rounded-full transition hover:bg-white/10 disabled:opacity-30 ${
								repeat !== "off"
									? "text-emerald-400"
									: "text-foreground/45"
							}`}
						>
							{repeat === "one" ? (
								<Repeat1 className="size-4" />
							) : (
								<Repeat className="size-4" />
							)}
						</button>
					</div>

					{/* Volume */}
					<div className="mt-3 flex items-center justify-center gap-2 border-t border-white/8 pt-3">
						<button
							type="button"
							title={isMuted ? "Unmute" : "Mute"}
							disabled={!hasSong}
							onClick={handleMute}
							className="flex size-7 shrink-0 items-center justify-center rounded-full text-foreground/50 transition hover:bg-white/10 hover:text-foreground disabled:opacity-30"
						>
							{isMuted || volume === 0 ? (
								<VolumeX className="size-4" />
							) : (
								<Volume2 className="size-4" />
							)}
						</button>

						<input
							type="range"
							min={0}
							max={1}
							step={0.01}
							value={isMuted ? 0 : volume}
							disabled={!hasSong}
							onChange={(event) =>
								handleVolumeChange(Number(event.target.value))
							}
							aria-label="Volume"
							className="h-1 w-32 max-w-full cursor-pointer accent-emerald-500 disabled:cursor-not-allowed disabled:opacity-30"
						/>

						<span className="w-8 shrink-0 text-xs tabular-nums text-foreground	/40">
							{Math.round((isMuted ? 0 : volume) * 100)}%
						</span>
					</div>
				</div>
			</div>
		</div>
	);
}

export default MusicPlayer;
