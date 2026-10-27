// src/lib/getMajorColors.ts

export async function getMajorColors(
	dataUrl: string | undefined,
	count = 3,
): Promise<string[]> {
	const fallback = ["17 17 19", "17 17 19", "17 17 19"];

	if (!dataUrl?.trim()) return fallback;

	try {
		const img = new Image();
		img.src = dataUrl;
		await img.decode();

		const canvas = document.createElement("canvas");
		const size = 100;
		canvas.width = size;
		canvas.height = size;

		const ctx = canvas.getContext("2d", {
			willReadFrequently: true,
		});

		if (!ctx) return fallback;

		ctx.drawImage(img, 0, 0, size, size);
		const { data } = ctx.getImageData(0, 0, size, size);
		const colors = new Map<string, number>();

		for (let i = 0; i < data.length; i += 4) {
			const [r, g, b, a] = [
				data[i],
				data[i + 1],
				data[i + 2],
				data[i + 3],
			];

			if (a < 128) continue;

			const key = [r, g, b]
				.map((v) => Math.min(255, Math.round(v / 32) * 32))
				.join(" ");

			colors.set(key, (colors.get(key) ?? 0) + 1);
		}

		const result = [...colors.entries()]
			.sort((a, b) => b[1] - a[1])
			.slice(0, count)
			.map(([rgb]) => rgb);

		return [...result, ...fallback].slice(0, count);
	} catch {
		return fallback;
	}
}
