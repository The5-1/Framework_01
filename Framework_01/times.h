#ifndef _TIMES_H
#define _TIMES_H

#ifdef _WIN32
#include <sys/timeb.h>
#include <sys/types.h>
#include <winsock2.h>

/* **************************************************************************************
0. Convert the Windows time-Commands to Linux standards
************************************************************************************** */
/*
Source for this Code: https://www.codefull.org/2015/12/systime-h-replacement-for-windows/
*/

int gettimeofday(timeval* t, void* timezone);

// from linux's sys/times.h

//#include <features.h>

#define __need_clock_t
#include <time.h>

/* Structure describing CPU time used by a process and its children.  */
struct tms
{
	clock_t tms_utime;          /* User CPU time.  */
	clock_t tms_stime;          /* System CPU time.  */

	clock_t tms_cutime;         /* User CPU time of dead children.  */
	clock_t tms_cstime;         /* System CPU time of dead children.  */
};

/* Store the CPU time used by this process and all its
dead children (and their dead children) in BUFFER.
Return the elapsed real time, or (clock_t) -1 for errors.
All times are in CLK_TCKths of a second.  */
clock_t times(tms *__buffer);

typedef long long suseconds_t;


/* **************************************************************************************
1. Timer Class to give current time and time steps
************************************************************************************** */

class Timer
{
public:
	Timer();
	void update();
	double intervall;
	double startTime;
	double currentTime = 0.0;

private:
	timeval tv;
};

#endif
#endif