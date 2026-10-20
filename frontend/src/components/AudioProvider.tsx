import { useEffect, useRef } from "react";
import usePlayerStore from "@/stores/playerStore";
import { registerAudioElement } from "@/lib/audioEngine";
import { playNextTrack } from "@/lib/playerControls";

function AudioProvider() {
	const audioRef = useRef<HTMLAudioElement>(null);
	const url = usePlayerStore((state) => state.url);
	const volume = usePlayerStore((state) => state.volume);
	const isMuted = usePlayerStore((state) => state.isMuted);
	// const repeat = usePlayerStore((state) => state.repeat);

	useEffect(() => {
		const audio = audioRef.current;
		registerAudioElement(audio);

		return () => {
			registerAudioElement(null);
		};
	}, []);

	useEffect(() => {
		const audio = audioRef.current;
		if (!audio || !url) return;

		// A new URL means a new track was selected. Load and start it.
		audio.load();
		void audio.play().catch((error: unknown) => {
			// Browsers/WebKit may reject playback; keep UI state accurate.
			console.error("[Audio] Unable to start playback:", error);
		});
	}, [url]);

	useEffect(() => {
		if (audioRef.current) audioRef.current.volume = volume;
	}, [volume]);

	useEffect(() => {
		if (audioRef.current) audioRef.current.muted = isMuted;
	}, [isMuted]);

	return (
		<audio
			ref={audioRef}
			src={url || undefined}
			preload="metadata"
			className="hidden"
			onPlay={() => usePlayerStore.getState().setIsPlaying(true)}
			onPause={() => usePlayerStore.getState().setIsPlaying(false)}
			onTimeUpdate={(event) =>
				usePlayerStore
					.getState()
					.setCurrentTime(event.currentTarget.currentTime)
			}
			onLoadedMetadata={(event) => {
				const duration = event.currentTarget.duration;
				if (Number.isFinite(duration)) {
					usePlayerStore.getState().setDuration(duration);
				}
			}}
			onDurationChange={(event) => {
				const duration = event.currentTarget.duration;
				if (Number.isFinite(duration)) {
					usePlayerStore.getState().setDuration(duration);
				}
			}}
			onEnded={() => {
				const { repeat } = usePlayerStore.getState();

				if (repeat === "one") {
					const audio = audioRef.current;
					if (!audio) return;

					audio.currentTime = 0;

					void audio.play().catch((error: unknown) => {
						console.error("[Audio] Repeat playback failed:", error);
					});

					return;
				}

				playNextTrack();
			}}
			onError={(event) => {
				console.error(
					"[Audio] Media error:",
					event.currentTarget.error,
					url,
				);
				usePlayerStore.getState().setIsPlaying(false);
			}}
		/>
	);
}

export default AudioProvider;
