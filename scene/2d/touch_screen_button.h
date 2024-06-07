/**************************************************************************/
/*  touch_screen_button.h                                                 */
/**************************************************************************/


#ifndef TOUCH_SCREEN_BUTTON_H
#define TOUCH_SCREEN_BUTTON_H

#include "scene/2d/node_2d.h"
#include "scene/resources/bit_map.h"
#include "scene/resources/rectangle_shape_2d.h"
#include "scene/resources/texture.h"

class TouchScreenButton : public Node2D {
	GDCLASS(TouchScreenButton, Node2D);

public:
	enum VisibilityMode {
		VISIBILITY_ALWAYS,
		VISIBILITY_TOUCHSCREEN_ONLY
	};

private:
	Ref<Texture> texture;
	Ref<Shape2D> shape;
	bool shape_centered;
	bool shape_visible;

	Ref<RectangleShape2D> unit_rect;

	StringName action;
	bool passby_press;
	int finger_pressed;

	VisibilityMode visibility;

	void _input(const Ref<InputEvent> &p_event);

	bool _is_point_inside(const Point2 &p_point);

	void _press(int p_finger_pressed);
	void _release(const Ref<InputEventScreenTouch> &p_touch_event, bool p_exiting_tree);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
#endif

	void set_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture() const;

	void set_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_shape() const;

	void set_shape_centered(bool p_shape_centered);
	bool is_shape_centered() const;

	void set_shape_visible(bool p_shape_visible);
	bool is_shape_visible() const;

	void set_action(const String &p_action);
	String get_action() const;

	void set_passby_press(bool p_enable);
	bool is_passby_press_enabled() const;

	void set_visibility_mode(VisibilityMode p_mode);
	VisibilityMode get_visibility_mode() const;

	bool is_pressed() const;

	virtual Rect2 get_anchorable_rect() const;

	TouchScreenButton();
};

VARIANT_ENUM_CAST(TouchScreenButton::VisibilityMode);

#endif // TOUCH_SCREEN_BUTTON_H
