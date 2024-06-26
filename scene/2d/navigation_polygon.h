/**************************************************************************/
/*  navigation_polygon.h                                                  */
/**************************************************************************/


#ifndef NAVIGATION_POLYGON_H
#define NAVIGATION_POLYGON_H

#include "scene/2d/node_2d.h"
#include "scene/resources/navigation_mesh.h"

class NavigationPolygon : public Resource {
	GDCLASS(NavigationPolygon, Resource);

	PoolVector<Vector2> vertices;
	struct Polygon {
		Vector<int> indices;
	};
	Vector<Polygon> polygons;
	Vector<PoolVector<Vector2>> outlines;

	mutable Rect2 item_rect;
	mutable bool rect_cache_dirty = true;

	Mutex navmesh_generation;
	// Navigation mesh
	Ref<NavigationMesh> navmesh;

protected:
	static void _bind_methods();

	void _set_polygons(const Array &p_array);
	Array _get_polygons() const;

	void _set_outlines(const Array &p_array);
	Array _get_outlines() const;

public:
#ifdef TOOLS_ENABLED
	Rect2 _edit_get_rect() const;
	bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const;
#endif

	void set_vertices(const PoolVector<Vector2> &p_vertices);
	PoolVector<Vector2> get_vertices() const;

	void add_polygon(const Vector<int> &p_polygon);
	int get_polygon_count() const;

	void add_outline(const PoolVector<Vector2> &p_outline);
	void add_outline_at_index(const PoolVector<Vector2> &p_outline, int p_index);
	void set_outline(int p_idx, const PoolVector<Vector2> &p_outline);
	PoolVector<Vector2> get_outline(int p_idx) const;
	void remove_outline(int p_idx);
	int get_outline_count() const;

	void clear_outlines();
	void make_polygons_from_outlines();

	Vector<int> get_polygon(int p_idx);
	void clear_polygons();

	Ref<NavigationMesh> get_mesh();

	NavigationPolygon();
	~NavigationPolygon();
};

class Navigation2D;

class NavigationPolygonInstance : public Node2D {
	GDCLASS(NavigationPolygonInstance, Node2D);

	bool enabled = true;
	RID region;
	Navigation2D *navigation = nullptr;
	Ref<NavigationPolygon> navpoly;

	real_t enter_cost = 0.0;
	real_t travel_cost = 1.0;

	uint32_t navigation_layers = 1;

	void _navpoly_changed();
	void _map_changed(RID p_map);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const;
#endif

	void set_enabled(bool p_enabled);
	bool is_enabled() const;

	void set_navigation_layers(uint32_t p_navigation_layers);
	uint32_t get_navigation_layers() const;

	RID get_region_rid() const;

	void set_enter_cost(real_t p_enter_cost);
	real_t get_enter_cost() const;

	void set_travel_cost(real_t p_travel_cost);
	real_t get_travel_cost() const;

	void set_navigation_polygon(const Ref<NavigationPolygon> &p_navpoly);
	Ref<NavigationPolygon> get_navigation_polygon() const;

	String get_configuration_warning() const;

	NavigationPolygonInstance();
	~NavigationPolygonInstance();
};

#endif // NAVIGATION_POLYGON_H
