/**************************************************************************/
/*  register_server_types.cpp                                             */
/**************************************************************************/


#include "register_server_types.h"
#include "core/engine.h"
#include "core/project_settings.h"

#include "scene/main/viewport.h"
#include "core/os/os.h"
#include "core/rid.h"
#include "core/variant.h"
#include "audio/audio_stream.h"
#include "audio_server.h"
#include "core/image.h"
#include "core/object.h"
#include "servers/visual_server.h"
#include "navigation_2d_server.h"
#include "navigation_server.h"
#include "physics_2d/physics_2d_server_sw.h"
#include "physics_2d/physics_2d_server_wrap_mt.h"
#include "physics_2d_server.h"
#include "scene/debugger/script_debugger_remote.h"
#include "visual/shader_types.h"
#include "servers/audio/effects/audio_effect_amplify.h"

static void _debugger_get_resource_usage(List<ScriptDebuggerRemote::ResourceUsage> *r_usage) {
	List<VS::TextureInfo> tinfo;
	VS::get_singleton()->texture_debug_usage(&tinfo);

	for (List<VS::TextureInfo>::Element *E = tinfo.front(); E; E = E->next()) {
		ScriptDebuggerRemote::ResourceUsage usage;
		usage.path = E->get().path;
		usage.vram = E->get().bytes;
		usage.id = E->get().texture;
		usage.type = "Texture";
		if (E->get().depth == 0) {
			usage.format = itos(E->get().width) + "x" + itos(E->get().height) + " " + Image::get_format_name(E->get().format);
		} else {
			usage.format = itos(E->get().width) + "x" + itos(E->get().height) + "x" + itos(E->get().depth) + " " + Image::get_format_name(E->get().format);
		}
		r_usage->push_back(usage);
	}
}

ShaderTypes *shader_types = nullptr;

Physics2DServer *_createGodotPhysics2DCallback() {
	return Physics2DServerWrapMT::init_server<Physics2DServerSW>();
}

static bool has_server_feature_callback(const String &p_feature) {
	if (VisualServer::get_singleton()) {
		if (VisualServer::get_singleton()->has_os_feature(p_feature)) {
			return true;
		}
	}

	return false;
}

void register_server_types() {
	OS::get_singleton()->set_has_server_feature_callback(has_server_feature_callback);

	ClassDB::register_virtual_class<VisualServer>();
	ClassDB::register_class<AudioServer>();
	ClassDB::register_virtual_class<Physics2DServer>();
	ClassDB::register_virtual_class<NavigationServer>();
	ClassDB::register_virtual_class<Navigation2DServer>();

	shader_types = memnew(ShaderTypes);

	ClassDB::register_virtual_class<AudioStream>();
	ClassDB::register_virtual_class<AudioStreamPlayback>();
	ClassDB::register_virtual_class<AudioStreamPlaybackResampled>();
	ClassDB::register_class<AudioBusLayout>();
	ClassDB::register_virtual_class<Physics2DDirectBodyState>();
	ClassDB::register_virtual_class<Physics2DDirectSpaceState>();
	ClassDB::register_class<Physics2DTestMotionResult>();
	ClassDB::register_class<Physics2DShapeQueryParameters>();

	ClassDB::register_class<AudioEffectAmplify>();

	ScriptDebuggerRemote::resource_usage_func = _debugger_get_resource_usage;

	// Physics 2D
	GLOBAL_DEF(Physics2DServerManager::setting_property_name, "DEFAULT");
	ProjectSettings::get_singleton()->set_custom_property_info(Physics2DServerManager::setting_property_name, PropertyInfo(Variant::STRING, Physics2DServerManager::setting_property_name, PROPERTY_HINT_ENUM, "DEFAULT"));

	Physics2DServerManager::register_server("GodotPhysics", &_createGodotPhysics2DCallback);
	Physics2DServerManager::set_default_server("GodotPhysics");

}

void unregister_server_types() {
	memdelete(shader_types);
}

void register_server_singletons() {
	Engine::get_singleton()->add_singleton(Engine::Singleton("VisualServer", VisualServer::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("AudioServer", AudioServer::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("Physics2DServer", Physics2DServer::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("NavigationServer", NavigationServer::get_singleton_mut()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("Navigation2DServer", Navigation2DServer::get_singleton_mut()));
}
