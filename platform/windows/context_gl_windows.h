/**************************************************************************/
/*  context_gl_windows.h                                                  */
/**************************************************************************/


#ifndef CONTEXT_GL_WINDOWS_H
#define CONTEXT_GL_WINDOWS_H

#if defined(OPENGL_ENABLED) || defined(GLES_ENABLED)

#include "core/error_list.h"
#include "core/os/os.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef bool(APIENTRY *PFNWGLSWAPINTERVALEXTPROC)(int interval);
typedef int(APIENTRY *PFNWGLGETSWAPINTERVALEXTPROC)(void);

class ContextGL_Windows {
	HDC hDC;
	HGLRC hRC;
	HGLRC hRC_offscreen;
	unsigned int pixel_format;
	HWND hWnd;
	bool opengl_3_context;
	bool use_vsync;
	bool vsync_via_compositor;

	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;

	static bool should_vsync_via_compositor();

public:
	void release_current();

	void make_current();

	bool is_offscreen_available() const;
	void make_offscreen_current();
	void release_offscreen_current();

	HDC get_hdc();
	HGLRC get_hglrc();

	int get_window_width();
	int get_window_height();
	void swap_buffers();

	Error initialize();

	void set_use_vsync(bool p_use);
	bool is_using_vsync() const;

	ContextGL_Windows(HWND hwnd, bool p_opengl_3_context);
	~ContextGL_Windows();
};

#endif

#endif // CONTEXT_GL_WINDOWS_H
