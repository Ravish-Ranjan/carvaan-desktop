import { useConfigureStore } from "@/stores/configureStore";
import { Input } from "../ui/input";
import Button from "../ui/button";
import {
	openPcMusicDirectoryDialog,
	setPhoneDestination,
	setPcMusicDestination,
} from "@/lib/configureBridge";
import { useEffect, useState } from "react";
import { Folder } from "lucide-react";

function Settings() {
	const { pcMusicDestination, phoneDestination } = useConfigureStore();
	const [pcMusicDir, setPcMusicDir] = useState<string>("");
	const [phoneDir, setPhoneDir] = useState<string>("");

	const handlePcMusicDirChange = () => {
		if (pcMusicDir.trim() !== pcMusicDestination) {
			setPcMusicDestination(pcMusicDir.trim());
		}
	};

	const handlePhoneDirChange = () => {
		if (phoneDir.trim() !== phoneDestination) {
			setPhoneDestination(phoneDir.trim());
		}
	};

	useEffect(() => {
		(function () {
			setPcMusicDir(pcMusicDestination);
			setPhoneDir(phoneDestination);
		})();
	}, [pcMusicDestination, phoneDestination]);

	return (
		<section>
			<h3 className="text-2xl font-semibold">Settings</h3>

			<p className="mt-1 text-sm text-muted-foreground">
				Configure your library, phone destination and synchronization
				behavior.
			</p>

			{/* folder locations */}
			<div className="grid p-4 gap-2 outline mt-4 rounded-2xl">
				<h3 className="font-semibold">Folder Locations</h3>
				<div className="flex gap-2 items-center">
					<span className="text-xs font-semibold">PC : </span>
					<Input
						value={pcMusicDir}
						onChange={(e) => setPcMusicDir(e.target.value)}
						placeholder="Select Music folder destination on PC"
						className="w-1/2"
					/>
					<Button onClick={handlePcMusicDirChange}>Set Folder</Button>
					<span className="text-gray-500">or</span>
					<Button
						onClick={openPcMusicDirectoryDialog}
						variant={"secondary"}
					>
						<Folder />
						Select Folder
					</Button>
				</div>
				<div className="flex gap-2 items-center">
					<span className="text-xs font-semibold">Phone : </span>
					<Input
						value={phoneDir}
						onChange={(e) => setPhoneDir(e.target.value)}
						placeholder="Select Music folder destination on Phone"
						className="w-1/2"
					/>
					<Button onClick={handlePhoneDirChange}>Set Folder</Button>
				</div>
			</div>

		</section>
	);
}

export default Settings;
