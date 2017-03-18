# include "Process.h"
// # include "util.h"
# include <string>
using namespace std;
using namespace util;

Process::Process(int pid, int at, int tc, int cb, int io, STATE stat, int prio)
    : pId(pid), AT(at), TC(tc), CB(cb), IO(io), curr_state(stat),
    curr_burst_begin(at), curr_burst_duration(0), rem_CPU_time(tc),
    static_priority(prio), dynamic_priority(prio - 1)
{}

int Process::getPID( void ) {
   return pId;
}

int Process::getAT( void ) {
   return AT;
}

int Process::getTC( void ) {
   return TC;
}

int Process::getCB( void ) {
   return CB;
}

int Process::getIO( void ) {
   return IO;
}

/*
string Process::getState( void ) {
   return state;
}

*/
