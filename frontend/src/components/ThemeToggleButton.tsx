import { useEffect, useState } from "react";
import Button from "./ui/button";
import { Monitor, MoonStar, Sun } from "lucide-react";

type ModeType = "system" | "dark" | "light";

const THEME_STORAGE_KEY = "theme";

function ThemeToggleButton() {
	const [mode, setMode] = useState<ModeType>(() => {
		const storedMode = window.localStorage.getItem(THEME_STORAGE_KEY);

		return storedMode === "dark" ||
			storedMode === "light" ||
			storedMode === "system"
			? storedMode
			: "system";
	});

	useEffect(() => {
		const mediaQuery = window.matchMedia("(prefers-color-scheme: dark)");

		const applyTheme = () => {
			const isDark =
				mode === "dark" || (mode === "system" && mediaQuery.matches);
			document.body.classList.toggle("dark", isDark);
		};

		applyTheme();

		if (mode === "system") {
			mediaQuery.addEventListener("change", applyTheme);
		}

		return () => {
			mediaQuery.removeEventListener("change", applyTheme);
		};
	}, [mode]);

	useEffect(() => {
		window.localStorage.setItem(THEME_STORAGE_KEY, mode);
	}, [mode]);

	const toggleMode = () => {
		setMode((currentMode) => {
			if (currentMode === "system") {
				return "dark";
			}

			if (currentMode === "dark") {
				return "light";
			}

			return "system";
		});
	};

	return (
		<Button
			type="button"
			variant="secondary"
			onClick={toggleMode}
			aria-label={`Theme: ${mode}. Switch theme`}
			title={`Theme: ${mode}. Switch theme`}
		>
			{mode === "system" && <Monitor />}
			{mode === "dark" && <MoonStar />}
			{mode === "light" && <Sun />}
		</Button>
	);
}

export default ThemeToggleButton;
