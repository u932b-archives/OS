#ifndef _PROCESS_H_
#define _PROCESS_H_
// #include <string>
# include "util.hpp"
using namespace util;

class Process
{
	public:
		Process(int, int, int, int, int, STATE, int);
		int getPID(void);
		int getAT(void);
		int getTC(void);
		int getCB(void);
		int getIO(void);
        STATE curr_state;
        int rem_CPU_time;
        int rem_burst;
        int static_priority;
        int dynamic_priority;
        int pId;
		int TC; // Total CPU Time
        int FT; // Finishing Time
        int TT; // Turnaround Time
        int IT; // I/O Time
        int CW; //CPU Waiting Time
        int enter_ready_time;

        int finishing_time;
        // int curr_burst_begin;
        // int curr_burst_duration;
        // int last_burst_duration;
        // int PosInQueue;
	private:
		int AT; // Arrival Time
		int CB; // CPU Burst
		int IO; // IO Burst

};

#endif

