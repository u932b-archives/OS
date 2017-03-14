#ifndef _PROCESS_H_
#define _PROCESS_H_
#include <string>
using namespace std;

class Process
{
	public:
		Process(int, int, int, int, string);
		int getAT(void);
		int getTC(void);
		int getCB(void);
		int getIO(void);
		string getState(void);
	private:
		int AT; // Arrival Time
		int TC; // Total CPU Time
		int CB; // CPU Burst
		int IO; // IO Burst
		string state;
};

#endif

