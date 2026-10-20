let audioElement: HTMLAudioElement | null = null;

export const registerAudioElement = (element: HTMLAudioElement | null) => {
	audioElement = element;
};

export const getAudioElement = () => audioElement;

export const playAudio = async () => {
	if (!audioElement) return;

	try {
		await audioElement.play();
	} catch (error) {
		console.error("[Audio] Playback failed:", error);
	}
};

export const pauseAudio = () => {
	audioElement?.pause();
};

export const toggleAudioPlayback = async () => {
	if (!audioElement) return;

	if (audioElement.paused) {
		await playAudio();
	} else {
		pauseAudio();
	}
};

export const seekAudio = (time: number) => {
	if (!audioElement || !Number.isFinite(time)) return;

	const upperBound = Number.isFinite(audioElement.duration)
		? audioElement.duration
		: time;

	audioElement.currentTime = Math.max(0, Math.min(time, upperBound));
};

export const setAudioVolume = (volume: number) => {
	if (!audioElement) return;

	audioElement.volume = Math.max(0, Math.min(1, volume));
};

export const setAudioMuted = (muted: boolean) => {
	if (audioElement) audioElement.muted = muted;
};

export default {
	registerAudioElement,
	getAudioElement,
	playAudio,
	pauseAudio,
	toggleAudioPlayback,
	seekAudio,
	setAudioMuted,
	setAudioVolume,
};
