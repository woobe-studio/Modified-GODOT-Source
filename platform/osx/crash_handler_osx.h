/**************************************************************************/
/*  crash_handler_osx.h                                                   */
/**************************************************************************/


#ifndef CRASH_HANDLER_OSX_H
#define CRASH_HANDLER_OSX_H

class CrashHandler {
	bool disabled;

public:
	void initialize();

	void disable();
	bool is_disabled() const { return disabled; };

	CrashHandler();
	~CrashHandler();
};

#endif // CRASH_HANDLER_OSX_H
