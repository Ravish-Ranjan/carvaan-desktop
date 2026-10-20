import type { Metadata } from "@/types/song";
import { parseBlob } from "music-metadata";

const KB = 1024;
const MB = KB * 1024;
const GB = MB * 1024;
const TB = GB * 1024;

export const sizeConverter = (size: number): string => {
	if (size < KB) {
		return `${size} B`;
	}
	if (size < MB) {
		return `${Math.round((size * 100) / KB) / 100} KB`;
	}
	if (size < GB) {
		return `${Math.round((size * 100) / MB) / 100} MB`;
	}
	if (size < TB) {
		return `${Math.round((size * 100) / GB) / 100} GB`;
	}
	return `${Math.round((size * 100) / TB) / 100} TB`;
};

export async function getMetadata(fileUrl: string,id:number): Promise<Metadata> {
	const res = await fetch(fileUrl);
	const blob = await res.blob();
	const { common, format } = await parseBlob(blob, { duration: true });

	const pic = common.picture?.[0];
	const coverArt = pic
		? URL.createObjectURL(
				new Blob([new Uint8Array(pic.data)], { type: pic.format }),
			)
		: undefined;

	return {
		id,
		title:
			common.title ||
			decodeURIComponent(
				(fileUrl.split("/").pop() ?? "").replace(/\.[^.]+$/, ""),
			),
		duration: format.duration ?? 0,
		coverArt,
		artist: common.artist ?? undefined,
		album: common.album ?? undefined,
		year: common.year ?? undefined,
	};
}
