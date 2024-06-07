/**************************************************************************/
/*  test_render.cpp                                                       */
/**************************************************************************/


#include "test_render.h"

#include "core/math/geometry.h"
#include "core/math/vector3.h"
#include "core/vector.h"
#include "core/math/math_funcs.h"
#include "core/os/main_loop.h"
#include "core/os/os.h"
#include "core/print_string.h"
#include "servers/visual_server.h"

#define OBJECT_COUNT 50

namespace TestRender {

class TestMainLoop : public MainLoop {
	RID test_cube;
	RID instance;
	RID camera;
	RID viewport;
	RID light;
	RID scenario;

	struct InstanceInfo {
		RID instance;
		Transform base;
		Vector3 rot_axis;
	};

	List<InstanceInfo> instances;

	float ofs;
	bool quit;

protected:
public:
	virtual void input_event(const Ref<InputEvent> &p_event) {
		if (p_event->is_pressed()) {
			quit = true;
		}
	}

	virtual void init() {
		print_line("INITIALIZING TEST RENDER");
		VisualServer *vs = VisualServer::get_singleton();
		test_cube = vs->get_test_cube();
		scenario = RID_PRIME(vs->scenario_create());

		Vector<Vector3> vts;

		vts.push_back(Vector3(1, 1, 1));
		vts.push_back(Vector3(1, -1, 1));
		vts.push_back(Vector3(-1, 1, 1));
		vts.push_back(Vector3(-1, -1, 1));
		vts.push_back(Vector3(1, 1, -1));
		vts.push_back(Vector3(1, -1, -1));
		vts.push_back(Vector3(-1, 1, -1));
		vts.push_back(Vector3(-1, -1, -1));

		Geometry::MeshData md;
		test_cube = RID_PRIME(vs->mesh_create());
		vs->mesh_add_surface_from_mesh_data(test_cube, md);

		List<String> cmdline = OS::get_singleton()->get_cmdline_args();
		int object_count = OBJECT_COUNT;
		if (cmdline.size() > 0 && cmdline[cmdline.size() - 1].to_int()) {
			object_count = cmdline[cmdline.size() - 1].to_int();
		};

		for (int i = 0; i < object_count; i++) {
			InstanceInfo ii;

			ii.instance = vs->instance_create2(test_cube, scenario);

			ii.base.translate(Math::random(-20, 20), Math::random(-20, 20), Math::random(-20, 18));
			ii.base.rotate(Vector3(0, 1, 0), Math::randf() * Math_PI);
			ii.base.rotate(Vector3(1, 0, 0), Math::randf() * Math_PI);
			vs->instance_set_transform(ii.instance, ii.base);

			ii.rot_axis = Vector3(Math::random(-1, 1), Math::random(-1, 1), Math::random(-1, 1)).normalized();

			instances.push_back(ii);
		}

		camera = RID_PRIME(vs->camera_create());

		// 		vs->camera_set_perspective( camera, 60.0,0.1, 100.0 );

		viewport = RID_PRIME(vs->viewport_create());
		Size2i screen_size = OS::get_singleton()->get_window_size();
		vs->viewport_set_size(viewport, screen_size.x, screen_size.y);
		vs->viewport_attach_to_screen(viewport, Rect2(Vector2(), screen_size));
		vs->viewport_set_active(viewport, true);
		vs->viewport_attach_camera(viewport, camera);
		vs->viewport_set_scenario(viewport, scenario);
		vs->camera_set_transform(camera, Transform(Basis(), Vector3(0, 3, 30)));
		vs->camera_set_perspective(camera, 60, 0.1, 1000);

		/*
		RID lightaux = vs->light_create( VisualServer::LIGHT_OMNI );
		vs->light_set_var( lightaux, VisualServer::LIGHT_VAR_RADIUS, 80 );
		vs->light_set_var( lightaux, VisualServer::LIGHT_VAR_ATTENUATION, 1 );
		vs->light_set_var( lightaux, VisualServer::LIGHT_VAR_ENERGY, 1.5 );
		light = vs->instance_create( lightaux );
		*/
		RID lightaux;

		lightaux = vs->directional_light_create();
		//vs->light_set_color( lightaux, VisualServer::LIGHT_COLOR_AMBIENT, Color(0.0,0.0,0.0) );
		vs->light_set_color(lightaux, Color(1.0, 1.0, 1.0));
		//vs->light_set_shadow( lightaux, true );
		light = vs->instance_create2(lightaux, scenario);
		Transform lla;
		//lla.set_look_at(Vector3(),Vector3(1,-1,1),Vector3(0,1,0));
		lla.set_look_at(Vector3(), Vector3(-0.000000, -0.836026, -0.548690), Vector3(0, 1, 0));

		vs->instance_set_transform(light, lla);

		lightaux = vs->omni_light_create();
		//vs->light_set_color( lightaux, VisualServer::LIGHT_COLOR_AMBIENT, Color(0.0,0.0,1.0) );
		vs->light_set_color(lightaux, Color(1.0, 1.0, 0.0));
		vs->light_set_param(lightaux, VisualServer::LIGHT_PARAM_RANGE, 4);
		vs->light_set_param(lightaux, VisualServer::LIGHT_PARAM_ENERGY, 8);
		//vs->light_set_shadow( lightaux, true );
		//light = vs->instance_create( lightaux );

		ofs = 0;
		quit = false;
	}
	virtual bool iteration(float p_time) {
		VisualServer *vs = VisualServer::get_singleton();
		//Transform t;
		//t.rotate(Vector3(0, 1, 0), ofs);
		//t.translate(Vector3(0,0,20 ));
		//vs->camera_set_transform(camera, t);

		ofs += p_time * 0.05;

		//return quit;

		for (List<InstanceInfo>::Element *E = instances.front(); E; E = E->next()) {
			Transform pre(Basis(E->get().rot_axis, ofs), Vector3());
			vs->instance_set_transform(E->get().instance, pre * E->get().base);
			/*
			if( !E->next() ) {

				vs->free( E->get().instance );
				instances.erase(E );
			}*/
		}

		return quit;
	}

	virtual bool idle(float p_time) {
		return quit;
	}

	virtual void finish() {
	}
};

MainLoop *test() {
	return memnew(TestMainLoop);
}
} // namespace TestRender
