/**************************************************************************/
/*  editor_preview_plugins.h                                              */
/**************************************************************************/


#ifndef EDITOR_PREVIEW_PLUGINS_H
#define EDITOR_PREVIEW_PLUGINS_H

#include "editor/editor_resource_preview.h"

#include "core/safe_refcount.h"

void post_process_preview(Ref<Image> p_image);

class EditorTexturePreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorTexturePreviewPlugin, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorTexturePreviewPlugin();
};

class EditorImagePreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorImagePreviewPlugin, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorImagePreviewPlugin();
};

class EditorBitmapPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorBitmapPreviewPlugin, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorBitmapPreviewPlugin();
};

class EditorPackedScenePreviewPlugin : public EditorResourcePreviewGenerator {
public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;
	virtual Ref<Texture> generate_from_path(const String &p_path, const Size2 &p_size) const;

	EditorPackedScenePreviewPlugin();
};

class EditorMaterialPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorMaterialPreviewPlugin, EditorResourcePreviewGenerator);

	RID scenario;
	RID sphere;
	RID sphere_instance;
	RID viewport;
	RID viewport_texture;
	RID light;
	RID light_instance;
	RID light2;
	RID light_instance2;
	RID camera;
	mutable SafeFlag preview_done;

	void _preview_done(const Variant &p_udata);

protected:
	static void _bind_methods();

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorMaterialPreviewPlugin();
	~EditorMaterialPreviewPlugin();
};

class EditorScriptPreviewPlugin : public EditorResourcePreviewGenerator {
public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorScriptPreviewPlugin();
};

class EditorAudioStreamPreviewPlugin : public EditorResourcePreviewGenerator {
public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorAudioStreamPreviewPlugin();
};

class EditorMeshPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorMeshPreviewPlugin, EditorResourcePreviewGenerator);

	RID scenario;
	RID mesh_instance;
	RID viewport;
	RID viewport_texture;
	RID light;
	RID light_instance;
	RID light2;
	RID light_instance2;
	RID camera;
	mutable SafeFlag preview_done;

	void _preview_done(const Variant &p_udata);

protected:
	static void _bind_methods();

public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorMeshPreviewPlugin();
	~EditorMeshPreviewPlugin();
};

class EditorFontPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorFontPreviewPlugin, EditorResourcePreviewGenerator);

	RID viewport;
	RID viewport_texture;
	RID canvas;
	RID canvas_item;
	mutable SafeFlag preview_done;

	void _preview_done(const Variant &p_udata);

protected:
	static void _bind_methods();

public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;
	virtual Ref<Texture> generate_from_path(const String &p_path, const Size2 &p_size) const;

	EditorFontPreviewPlugin();
	~EditorFontPreviewPlugin();
};

#endif // EDITOR_PREVIEW_PLUGINS_H
