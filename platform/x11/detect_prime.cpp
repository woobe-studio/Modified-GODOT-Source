/**************************************************************************/
/*  detect_prime.cpp                                                      */
/**************************************************************************/


#ifdef X11_ENABLED
#if defined(OPENGL_ENABLED)

#include "detect_prime.h"

#include "core/print_string.h"
#include "core/ustring.h"

#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

struct vendor {
	const char *glxvendor;
	int priority = 0;
};

vendor vendormap[] = {
	{ "Advanced Micro Devices, Inc.", 30 },
	{ "AMD", 30 },
	{ "NVIDIA Corporation", 30 },
	{ "X.Org", 30 },
	{ "Intel Open Source Technology Center", 20 },
	{ "Intel", 20 },
	{ "nouveau", 10 },
	{ "Mesa Project", 0 },
	{ nullptr, 0 }
};

// Runs inside a child. Exiting will not quit the engine.
void create_context() {
	Display *x11_display = XOpenDisplay(nullptr);
	Window x11_window;
	GLXContext glx_context;

	GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");

	static int visual_attribs[] = {
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_DOUBLEBUFFER, true,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DEPTH_SIZE, 24,
		None
	};

	int fbcount;
	GLXFBConfig fbconfig = nullptr;
	XVisualInfo *vi = nullptr;

	XSetWindowAttributes swa;
	swa.event_mask = StructureNotifyMask;
	swa.border_pixel = 0;
	unsigned long valuemask = CWBorderPixel | CWColormap | CWEventMask;

	GLXFBConfig *fbc = glXChooseFBConfig(x11_display, DefaultScreen(x11_display), visual_attribs, &fbcount);
	if (!fbc) {
		exit(1);
	}

	vi = glXGetVisualFromFBConfig(x11_display, fbc[0]);

	fbconfig = fbc[0];

	static int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	};

	glx_context = glXCreateContextAttribsARB(x11_display, fbconfig, nullptr, true, context_attribs);

	swa.colormap = XCreateColormap(x11_display, RootWindow(x11_display, vi->screen), vi->visual, AllocNone);
	x11_window = XCreateWindow(x11_display, RootWindow(x11_display, vi->screen), 0, 0, 10, 10, 0, vi->depth, InputOutput, vi->visual, valuemask, &swa);

	if (!x11_window) {
		exit(1);
	}

	glXMakeCurrent(x11_display, x11_window, glx_context);
	XFree(vi);
}

int detect_prime() {
	pid_t p;
	int priorities[2] = {};
	String vendors[2];
	String renderers[2];

	vendors[0] = "Unknown";
	vendors[1] = "Unknown";
	renderers[0] = "Unknown";
	renderers[1] = "Unknown";

	for (int i = 0; i < 2; ++i) {
		int fdset[2];

		if (pipe(fdset) == -1) {
			print_verbose("Failed to pipe(), using default GPU");
			return 0;
		}

		// Fork so the driver initialization can crash without taking down the engine.
		p = fork();

		if (p > 0) {
			// Main thread

			int stat_loc = 0;
			char string[201];
			string[200] = '\0';

			close(fdset[1]);

			waitpid(p, &stat_loc, 0);

			if (!stat_loc) {
				// No need to do anything complicated here. Anything less than
				// PIPE_BUF will be delivered in one read() call.
				// Leave it 'Unknown' otherwise.
				if (read(fdset[0], string, sizeof(string) - 1) > 0) {
					vendors[i] = string;
					renderers[i] = string + strlen(string) + 1;
				}
			}

			close(fdset[0]);

		} else {
			// In child, exit() here will not quit the engine.

			char string[201];

			close(fdset[0]);

			if (i) {
				setenv("DRI_PRIME", "1", 1);
			}
			create_context();

			const char *vendor = (const char *)glGetString(GL_VENDOR);
			const char *renderer = (const char *)glGetString(GL_RENDERER);

			unsigned int vendor_len = strlen(vendor) + 1;
			unsigned int renderer_len = strlen(renderer) + 1;

			if (vendor_len + renderer_len >= sizeof(string)) {
				renderer_len = 200 - vendor_len;
			}

			memcpy(&string, vendor, vendor_len);
			memcpy(&string[vendor_len], renderer, renderer_len);

			if (write(fdset[1], string, vendor_len + renderer_len) == -1) {
				print_verbose("Couldn't write vendor/renderer string.");
			}
			close(fdset[1]);

			// The function quick_exit() is used because exit() will call destructors on static objects copied by fork().
			// These objects will be freed anyway when the process finishes execution.
			quick_exit(0);
		}
	}

	int preferred = 0;
	int priority = 0;

	if (vendors[0] == vendors[1]) {
		print_verbose("Only one GPU found, using default.");
		return 0;
	}

	for (int i = 1; i >= 0; --i) {
		vendor *v = vendormap;
		while (v->glxvendor) {
			if (v->glxvendor == vendors[i]) {
				priorities[i] = v->priority;

				if (v->priority >= priority) {
					priority = v->priority;
					preferred = i;
				}
			}
			++v;
		}
	}

	print_verbose("Found renderers:");
	for (int i = 0; i < 2; ++i) {
		print_verbose("Renderer " + itos(i) + ": " + renderers[i] + " with priority: " + itos(priorities[i]));
	}

	print_verbose("Using renderer: " + renderers[preferred]);
	return preferred;
}

#endif
#endif
