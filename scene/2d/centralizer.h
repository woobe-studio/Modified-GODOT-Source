/**************************************************************************/
/*  centralizer.h                                                             */
/**************************************************************************/


#ifndef CENTRALIZER_H
#define CENTRALIZER_H

#include "scene/2d/node_2d.h"

class Centralizer : public Node2D {
GDCLASS(Centralizer, Node2D);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    enum LayoutPreset {
        PRESET_TOP_LEFT,
        PRESET_TOP_RIGHT,
        PRESET_BOTTOM_LEFT,
        PRESET_BOTTOM_RIGHT,
        PRESET_CENTER,
    };

    LayoutPreset current_preset;

    void set_position_preset(LayoutPreset p_preset);
    int get_position_preset() const;

    void update_position();

    Centralizer();
    ~Centralizer();
};

VARIANT_ENUM_CAST(Centralizer::LayoutPreset);

#endif // CENTRALIZER_H
