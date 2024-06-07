/**************************************************************************/
/*  crash_handler_x11.h                                                   */
/**************************************************************************/


#ifndef CRASH_HANDLER_X11_H
#define CRASH_HANDLER_X11_H

class CrashHandler {
	bool disabled;

public:
	void initialize();

	void disable();
	bool is_disabled() const { return disabled; };

	CrashHandler();
	~CrashHandler();
};

#endif // CRASH_HANDLER_X11_H
