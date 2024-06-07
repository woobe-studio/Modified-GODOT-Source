/**************************************************************************/
/*  context_gl_x11.h                                                      */
/**************************************************************************/


#ifndef CONTEXT_GL_X11_H
#define CONTEXT_GL_X11_H

#ifdef X11_ENABLED

#if defined(OPENGL_ENABLED)

#include "core/os/os.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>

struct ContextGL_X11_Private;

class ContextGL_X11 {
public:
	enum ContextType {
		OLDSTYLE,
		GLES_2_0_COMPATIBLE,
		GLES_3_0_COMPATIBLE
	};

private:
	ContextGL_X11_Private *p;
	OS::VideoMode default_video_mode;
	//::Colormap x11_colormap;
	::Display *x11_display;
	::Window &x11_window;
	bool double_buffer;
	bool direct_render;
	int glx_minor, glx_major;
	bool use_vsync;
	ContextType context_type;

public:
	void release_current();
	void make_current();
	void swap_buffers();
	int get_window_width();
	int get_window_height();
	void *get_glx_context();

	bool is_offscreen_available() const;
	void make_offscreen_current();
	void release_offscreen_current();

	Error initialize();

	void set_use_vsync(bool p_use);
	bool is_using_vsync() const;

	ContextGL_X11(::Display *p_x11_display, ::Window &p_x11_window, const OS::VideoMode &p_default_video_mode, ContextType p_context_type);
	~ContextGL_X11();
};

#endif

#endif

#endif // CONTEXT_GL_X11_H
