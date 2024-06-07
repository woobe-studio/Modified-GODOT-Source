/**************************************************************************/
/*  shape.h                                                               */
/**************************************************************************/


#ifndef SHAPE_H
#define SHAPE_H

#include "core/resource.h"
class ArrayMesh;

class Shape : public Resource {
	GDCLASS(Shape, Resource);
	OBJ_SAVE_TYPE(Shape);
	RES_BASE_EXTENSION("shape");
	RID shape;
	real_t margin;

	Ref<ArrayMesh> debug_mesh_cache;

protected:
	static void _bind_methods();

	_FORCE_INLINE_ RID get_shape() const { return shape; }
	Shape(RID p_shape);

	virtual void _update_shape();

public:
	virtual RID get_rid() const { return shape; }

	Ref<ArrayMesh> get_debug_mesh();
	virtual Vector<Vector3> get_debug_mesh_lines() = 0; // { return Vector<Vector3>(); }
	/// Returns the radius of a sphere that fully enclose this shape
	virtual real_t get_enclosing_radius() const = 0;

	real_t get_margin() const;
	void set_margin(real_t p_margin);

	Shape();
	~Shape();
};

#endif // SHAPE_H
