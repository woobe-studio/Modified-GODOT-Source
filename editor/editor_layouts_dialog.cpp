/**************************************************************************/
/*  editor_layouts_dialog.cpp                                             */
/**************************************************************************/


#include "editor_layouts_dialog.h"
#include "core/class_db.h"
#include "core/io/config_file.h"
#include "core/os/keyboard.h"
#include "editor/editor_settings.h"
#include "scene/gui/item_list.h"
#include "scene/gui/line_edit.h"

void EditorLayoutsDialog::_line_gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventKey> k = p_event;

	if (k.is_valid()) {
		if (!k->is_pressed()) {
			return;
		}

		switch (k->get_scancode()) {
			case KEY_KP_ENTER:
			case KEY_ENTER: {
				if (get_hide_on_ok()) {
					hide();
				}
				ok_pressed();
				accept_event();
			} break;
			case KEY_ESCAPE: {
				hide();
				accept_event();
			} break;
		}
	}
}

void EditorLayoutsDialog::_bind_methods() {
	ClassDB::bind_method("_line_gui_input", &EditorLayoutsDialog::_line_gui_input);

	ADD_SIGNAL(MethodInfo("name_confirmed", PropertyInfo(Variant::STRING, "name")));
}

void EditorLayoutsDialog::ok_pressed() {
	if (layout_names->is_anything_selected()) {
		Vector<int> const selected_items = layout_names->get_selected_items();
		for (int i = 0; i < selected_items.size(); ++i) {
			emit_signal("name_confirmed", layout_names->get_item_text(selected_items[i]));
		}
	} else if (name->is_visible() && name->get_text() != "") {
		emit_signal("name_confirmed", name->get_text());
	}
}

void EditorLayoutsDialog::_post_popup() {
	ConfirmationDialog::_post_popup();
	name->clear();
	layout_names->clear();

	Ref<ConfigFile> config;
	config.instance();
	Error err = config->load(EditorSettings::get_singleton()->get_editor_layouts_config());
	if (err != OK) {
		return;
	}

	List<String> layouts;
	config.ptr()->get_sections(&layouts);

	for (List<String>::Element *E = layouts.front(); E; E = E->next()) {
		layout_names->add_item(**E);
	}
}

EditorLayoutsDialog::EditorLayoutsDialog() {
	makevb = memnew(VBoxContainer);
	add_child(makevb);
	makevb->set_anchor_and_margin(MARGIN_LEFT, ANCHOR_BEGIN, 5);
	makevb->set_anchor_and_margin(MARGIN_RIGHT, ANCHOR_END, -5);

	layout_names = memnew(ItemList);
	makevb->add_child(layout_names);
	layout_names->set_visible(true);
	layout_names->set_margin(MARGIN_TOP, 5);
	layout_names->set_anchor_and_margin(MARGIN_LEFT, ANCHOR_BEGIN, 5);
	layout_names->set_anchor_and_margin(MARGIN_RIGHT, ANCHOR_END, -5);
	layout_names->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	layout_names->set_select_mode(ItemList::SELECT_MULTI);
	layout_names->set_allow_rmb_select(true);

	name = memnew(LineEdit);
	makevb->add_child(name);
	name->set_margin(MARGIN_TOP, 5);
	name->set_anchor_and_margin(MARGIN_LEFT, ANCHOR_BEGIN, 5);
	name->set_anchor_and_margin(MARGIN_RIGHT, ANCHOR_END, -5);
	name->connect("gui_input", this, "_line_gui_input");
	name->connect("focus_entered", layout_names, "unselect_all");
}

void EditorLayoutsDialog::set_name_line_enabled(bool p_enabled) {
	name->set_visible(p_enabled);
}
