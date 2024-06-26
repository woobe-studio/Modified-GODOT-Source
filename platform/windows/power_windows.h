/**************************************************************************/
/*  power_windows.h                                                       */
/**************************************************************************/


#ifndef POWER_WINDOWS_H
#define POWER_WINDOWS_H

#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/os/os.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class PowerWindows {
private:
	int nsecs_left;
	int percent_left;
	OS::PowerState power_state;

	bool GetPowerInfo_Windows();

public:
	PowerWindows();
	virtual ~PowerWindows();

	OS::PowerState get_power_state();
	int get_power_seconds_left();
	int get_power_percent_left();
};

#endif // POWER_WINDOWS_H
