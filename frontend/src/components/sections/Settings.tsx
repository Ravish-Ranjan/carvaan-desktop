import { useConfigureStore } from "@/stores/configureStore";

function Settings() {
	const { pcMusicDestination, phoneDestination } = useConfigureStore();
	return (
		<section>
			<h3 className="text-2xl font-semibold">Settings</h3>

			<p className="mt-1 text-sm text-muted-foreground">
				Configure your library, phone destination and synchronization
				behavior.
			</p>

			<div>
				{pcMusicDestination}
				{phoneDestination}
			</div>
		</section>
	);
}

export default Settings;
