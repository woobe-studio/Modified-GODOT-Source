/**************************************************************************/
/*  groups_editor.h                                                       */
/**************************************************************************/


#ifndef GROUPS_EDITOR_H
#define GROUPS_EDITOR_H

#include "core/undo_redo.h"
#include "editor/scene_tree_editor.h"
#include "scene/gui/button.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/item_list.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/popup.h"
#include "scene/gui/tool_button.h"
#include "scene/gui/tree.h"

class GroupDialog : public WindowDialog {
	GDCLASS(GroupDialog, WindowDialog);

	ConfirmationDialog *error;

	SceneTree *scene_tree;
	TreeItem *groups_root;

	LineEdit *add_group_text;
	Button *add_group_button;

	Tree *groups;

	Tree *nodes_to_add;
	TreeItem *add_node_root;
	LineEdit *add_filter;

	Tree *nodes_to_remove;
	TreeItem *remove_node_root;
	LineEdit *remove_filter;

	Label *group_empty;

	ToolButton *add_button;
	ToolButton *remove_button;

	String selected_group;

	UndoRedo *undo_redo;

	void _group_selected();

	void _remove_filter_changed(const String &p_filter);
	void _add_filter_changed(const String &p_filter);

	void _add_pressed();
	void _removed_pressed();
	void _add_group_pressed(const String &p_name);
	void _add_group_text_changed(const String &p_new_text);

	void _group_renamed();
	void _rename_group_item(const String &p_old_name, const String &p_new_name);

	void _add_group(String p_name);
	void _modify_group_pressed(Object *p_item, int p_column, int p_id);
	void _delete_group_item(const String &p_name);

	bool _can_edit(Node *p_node, String p_group);

	void _load_groups(Node *p_current);
	void _load_nodes(Node *p_current);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	enum ModifyButton {
		DELETE_GROUP,
		COPY_GROUP,
	};

	void edit();
	void set_undo_redo(UndoRedo *p_undoredo) { undo_redo = p_undoredo; }

	GroupDialog();
};

class GroupsEditor : public VBoxContainer {
	GDCLASS(GroupsEditor, VBoxContainer);

	Node *node;

	GroupDialog *group_dialog;

	LineEdit *group_name;
	Button *add;
	Tree *tree;

	UndoRedo *undo_redo;

	void update_tree();
	void _add_group(const String &p_group = "");
	void _modify_group(Object *p_item, int p_column, int p_id);
	void _group_name_changed(const String &p_new_text);

	void _show_group_dialog();

protected:
	static void _bind_methods();

public:
	enum ModifyButton {
		DELETE_GROUP,
		COPY_GROUP,
	};

	void set_undo_redo(UndoRedo *p_undoredo) { undo_redo = p_undoredo; }
	void set_current(Node *p_node);

	GroupsEditor();
	~GroupsEditor();
};

#endif // GROUPS_EDITOR_H
