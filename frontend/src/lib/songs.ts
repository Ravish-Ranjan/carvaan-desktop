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

