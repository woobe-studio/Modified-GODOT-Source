/**************************************************************************/
/*  curve.h                                                               */
/**************************************************************************/


#ifndef CURVE_H
#define CURVE_H

#include "core/resource.h"

// y(x) curve
class Curve : public Resource {
	GDCLASS(Curve, Resource);

public:
	static const int MIN_X = 0.f;
	static const int MAX_X = 1.f;

	static const char *SIGNAL_RANGE_CHANGED;

	enum TangentMode {
		TANGENT_FREE = 0,
		TANGENT_LINEAR,
		TANGENT_MODE_COUNT
	};

	struct Point {
		Vector2 pos;
		real_t left_tangent;
		real_t right_tangent;
		TangentMode left_mode;
		TangentMode right_mode;

		Point() {
			left_tangent = 0;
			right_tangent = 0;
			left_mode = TANGENT_FREE;
			right_mode = TANGENT_FREE;
		}

		Point(Vector2 p_pos,
				real_t p_left = 0,
				real_t p_right = 0,
				TangentMode p_left_mode = TANGENT_FREE,
				TangentMode p_right_mode = TANGENT_FREE) {
			pos = p_pos;
			left_tangent = p_left;
			right_tangent = p_right;
			left_mode = p_left_mode;
			right_mode = p_right_mode;
		}
	};

	Curve();

	int get_point_count() const { return _points.size(); }

	int add_point(Vector2 p_pos,
			real_t left_tangent = 0,
			real_t right_tangent = 0,
			TangentMode left_mode = TANGENT_FREE,
			TangentMode right_mode = TANGENT_FREE);

	void remove_point(int p_index);
	void clear_points();

	int get_index(real_t offset) const;

	void set_point_value(int p_index, real_t pos);
	int set_point_offset(int p_index, float offset);
	Vector2 get_point_position(int p_index) const;

	Point get_point(int p_index) const;

	float get_min_value() const { return _min_value; }
	void set_min_value(float p_min);

	float get_max_value() const { return _max_value; }
	void set_max_value(float p_max);

	real_t interpolate(real_t offset) const;
	real_t interpolate_local_nocheck(int index, real_t local_offset) const;

	void clean_dupes();

	void set_point_left_tangent(int i, real_t tangent);
	void set_point_right_tangent(int i, real_t tangent);
	void set_point_left_mode(int i, TangentMode p_mode);
	void set_point_right_mode(int i, TangentMode p_mode);

	real_t get_point_left_tangent(int i) const;
	real_t get_point_right_tangent(int i) const;
	TangentMode get_point_left_mode(int i) const;
	TangentMode get_point_right_mode(int i) const;

	void update_auto_tangents(int i);

	Array get_data() const;
	void set_data(Array input);

	void bake();
	int get_bake_resolution() const { return _bake_resolution; }
	void set_bake_resolution(int p_resolution);
	real_t interpolate_baked(real_t offset);

	void ensure_default_setup(float p_min, float p_max);

protected:
	static void _bind_methods();

private:
	void mark_dirty();

	Vector<Point> _points;
	bool _baked_cache_dirty;
	Vector<real_t> _baked_cache;
	int _bake_resolution;
	float _min_value;
	float _max_value;
	int _minmax_set_once; // Encodes whether min and max have been set a first time, first bit for min and second for max.
};

VARIANT_ENUM_CAST(Curve::TangentMode)

class Curve2D : public Resource {
	GDCLASS(Curve2D, Resource);

	struct Point {
		Vector2 in;
		Vector2 out;
		Vector2 pos;
	};

	Vector<Point> points;

	mutable bool baked_cache_dirty;
	mutable PoolVector2Array baked_point_cache;
	mutable float baked_max_ofs;

	void _bake() const;

	float bake_interval;

	void _bake_segment2d(Map<float, Vector2> &r_bake, float p_begin, float p_end, const Vector2 &p_a, const Vector2 &p_out, const Vector2 &p_b, const Vector2 &p_in, int p_depth, int p_max_depth, float p_tol) const;
	Dictionary _get_data() const;
	void _set_data(const Dictionary &p_data);

protected:
	static void _bind_methods();

public:
	int get_point_count() const;
	void add_point(const Vector2 &p_pos, const Vector2 &p_in = Vector2(), const Vector2 &p_out = Vector2(), int p_atpos = -1);
	void set_point_position(int p_index, const Vector2 &p_pos);
	Vector2 get_point_position(int p_index) const;
	void set_point_in(int p_index, const Vector2 &p_in);
	Vector2 get_point_in(int p_index) const;
	void set_point_out(int p_index, const Vector2 &p_out);
	Vector2 get_point_out(int p_index) const;
	void remove_point(int p_index);
	void clear_points();

	Vector2 interpolate(int p_index, float p_offset) const;
	Vector2 interpolatef(real_t p_findex) const;

	void set_bake_interval(float p_tolerance);
	float get_bake_interval() const;

	float get_baked_length() const;
	Vector2 interpolate_baked(float p_offset, bool p_cubic = false) const;
	PoolVector2Array get_baked_points() const; //useful for going through
	Vector2 get_closest_point(const Vector2 &p_to_point) const;
	float get_closest_offset(const Vector2 &p_to_point) const;

	PoolVector2Array tessellate(int p_max_stages = 5, float p_tolerance = 4) const; //useful for display

	Curve2D();
};

#endif // CURVE_H
