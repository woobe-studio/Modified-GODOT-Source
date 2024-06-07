/**************************************************************************/
/*  export.cpp                                                            */
/**************************************************************************/


#include "export.h"

#include "export_plugin.h"

void register_android_exporter() {
	EDITOR_DEF("export/android/android_sdk_path", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/android/android_sdk_path", PROPERTY_HINT_GLOBAL_DIR));
	EDITOR_DEF("export/android/debug_keystore", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/android/debug_keystore", PROPERTY_HINT_GLOBAL_FILE, "*.keystore,*.jks"));
	EDITOR_DEF("export/android/debug_keystore_user", "androiddebugkey");
	EDITOR_DEF("export/android/debug_keystore_pass", "android");
	EDITOR_DEF("export/android/force_system_user", false);

	EDITOR_DEF("export/android/shutdown_adb_on_exit", true);

	Ref<EditorExportPlatformAndroid> exporter = Ref<EditorExportPlatformAndroid>(memnew(EditorExportPlatformAndroid));
	EditorExport::get_singleton()->add_export_platform(exporter);
}
