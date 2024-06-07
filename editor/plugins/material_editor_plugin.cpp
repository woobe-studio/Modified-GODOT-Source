/**************************************************************************/
/*  material_editor_plugin.cpp                                            */
/**************************************************************************/


#include "material_editor_plugin.h"

#include "editor/editor_scale.h"
#include "scene/gui/viewport_container.h"

void MaterialEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		//get_scene()->connect("node_removed",this,"_node_removed");

		if (first_enter) {
			//it's in propertyeditor so.. could be moved around

			light_1_switch->set_normal_texture(get_icon("MaterialPreviewLight1", "EditorIcons"));
			light_1_switch->set_pressed_texture(get_icon("MaterialPreviewLight1Off", "EditorIcons"));
			light_2_switch->set_normal_texture(get_icon("MaterialPreviewLight2", "EditorIcons"));
			light_2_switch->set_pressed_texture(get_icon("MaterialPreviewLight2Off", "EditorIcons"));

			sphere_switch->set_normal_texture(get_icon("MaterialPreviewSphereOff", "EditorIcons"));
			sphere_switch->set_pressed_texture(get_icon("MaterialPreviewSphere", "EditorIcons"));
			box_switch->set_normal_texture(get_icon("MaterialPreviewCubeOff", "EditorIcons"));
			box_switch->set_pressed_texture(get_icon("MaterialPreviewCube", "EditorIcons"));

			first_enter = false;
		}
	}

	if (p_what == NOTIFICATION_DRAW) {
		Ref<Texture> checkerboard = get_icon("Checkerboard", "EditorIcons");
		Size2 size = get_size();

		draw_texture_rect(checkerboard, Rect2(Point2(), size), true);
	}
}

void MaterialEditor::edit(Ref<Material> p_material, const Ref<Environment> &p_env) {
	material = p_material;
	if (!material.is_null()) {
	} else {
		hide();
	}
}

void MaterialEditor::_button_pressed(Node *p_button) {

	if (p_button == box_switch) {
		box_switch->set_pressed(true);
		sphere_switch->set_pressed(false);
		EditorSettings::get_singleton()->set_project_metadata("inspector_options", "material_preview_on_sphere", false);
	}

	if (p_button == sphere_switch) {
		box_switch->set_pressed(false);
		sphere_switch->set_pressed(true);
		EditorSettings::get_singleton()->set_project_metadata("inspector_options", "material_preview_on_sphere", true);
	}
}

void MaterialEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_button_pressed"), &MaterialEditor::_button_pressed);
}

MaterialEditor::MaterialEditor() {
	vc = memnew(ViewportContainer);
	vc->set_stretch(true);
	add_child(vc);
	vc->set_anchors_and_margins_preset(PRESET_WIDE);
	viewport = memnew(Viewport);
	Ref<World> world;
	world.instance();
	viewport->set_world(world); //use own world
	vc->add_child(viewport);
	viewport->set_disable_input(true);
	viewport->set_transparent_background(true);
	viewport->set_msaa(Viewport::MSAA_4X);

	set_custom_minimum_size(Size2(1, 150) * EDSCALE);

	HBoxContainer *hb = memnew(HBoxContainer);
	add_child(hb);
	hb->set_anchors_and_margins_preset(Control::PRESET_WIDE, Control::PRESET_MODE_MINSIZE, 2);

	VBoxContainer *vb_shape = memnew(VBoxContainer);
	hb->add_child(vb_shape);

	sphere_switch = memnew(TextureButton);
	sphere_switch->set_toggle_mode(true);
	sphere_switch->set_pressed(true);
	vb_shape->add_child(sphere_switch);
	sphere_switch->connect("pressed", this, "_button_pressed", varray(sphere_switch));

	box_switch = memnew(TextureButton);
	box_switch->set_toggle_mode(true);
	box_switch->set_pressed(false);
	vb_shape->add_child(box_switch);
	box_switch->connect("pressed", this, "_button_pressed", varray(box_switch));

	hb->add_spacer();

	VBoxContainer *vb_light = memnew(VBoxContainer);
	hb->add_child(vb_light);

	light_1_switch = memnew(TextureButton);
	light_1_switch->set_toggle_mode(true);
	vb_light->add_child(light_1_switch);
	light_1_switch->connect("pressed", this, "_button_pressed", varray(light_1_switch));

	light_2_switch = memnew(TextureButton);
	light_2_switch->set_toggle_mode(true);
	vb_light->add_child(light_2_switch);
	light_2_switch->connect("pressed", this, "_button_pressed", varray(light_2_switch));

	first_enter = true;

	if (EditorSettings::get_singleton()->get_project_metadata("inspector_options", "material_preview_on_sphere", true)) {
	} else {
		box_switch->set_pressed(true);
		sphere_switch->set_pressed(false);
	}
}

///////////////////////

bool EditorInspectorPluginMaterial::can_handle(Object *p_object) {
	Material *material = Object::cast_to<Material>(p_object);
	if (!material) {
		return false;
	}

	return material->get_shader_mode() == Shader::MODE_SPATIAL;
}

void EditorInspectorPluginMaterial::parse_begin(Object *p_object) {
	Material *material = Object::cast_to<Material>(p_object);
	if (!material) {
		return;
	}
	Ref<Material> m(material);

	MaterialEditor *editor = memnew(MaterialEditor);
	editor->edit(m, env);
	add_custom_control(editor);
}

EditorInspectorPluginMaterial::EditorInspectorPluginMaterial() {
	env.instance();
	env->set_background(Environment::BG_COLOR_SKY);
}

MaterialEditorPlugin::MaterialEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginMaterial> plugin;
	plugin.instance();
	add_inspector_plugin(plugin);
}

String SpatialMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}
bool SpatialMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<SpatialMaterial> mat = p_resource;
	return mat.is_valid();
}
Ref<Resource> SpatialMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<SpatialMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instance();

	Ref<Shader> shader;
	shader.instance();

	String code = VS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	VS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (List<PropertyInfo>::Element *E = params.front(); E; E = E->next()) {
		// Texture parameter has to be treated specially since SpatialMaterial saved it
		// as RID but ShaderMaterial needs Texture itself
		Ref<Texture> texture = mat->get_texture_by_name(E->get().name);
		if (texture.is_valid()) {
			smat->set_shader_param(E->get().name, texture);
		} else {
			Variant value = VS::get_singleton()->material_get_param(mat->get_rid(), E->get().name);
			smat->set_shader_param(E->get().name, value);
		}
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String CanvasItemMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}
bool CanvasItemMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<CanvasItemMaterial> mat = p_resource;
	return mat.is_valid();
}
Ref<Resource> CanvasItemMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<CanvasItemMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instance();

	Ref<Shader> shader;
	shader.instance();

	String code = VS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	VS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (List<PropertyInfo>::Element *E = params.front(); E; E = E->next()) {
		Variant value = VS::get_singleton()->material_get_param(mat->get_rid(), E->get().name);
		smat->set_shader_param(E->get().name, value);
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}
