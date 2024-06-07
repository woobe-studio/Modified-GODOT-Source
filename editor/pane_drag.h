/**************************************************************************/
/*  pane_drag.h                                                           */
/**************************************************************************/


#ifndef PANE_DRAG_H
#define PANE_DRAG_H

#include "scene/gui/control.h"

class PaneDrag : public Control {
	GDCLASS(PaneDrag, Control);

	bool mouse_over;

protected:
	void _gui_input(const Ref<InputEvent> &p_input);
	void _notification(int p_what);
	virtual Size2 get_minimum_size() const;
	static void _bind_methods();

public:
	PaneDrag();
};

#endif // PANE_DRAG_H
