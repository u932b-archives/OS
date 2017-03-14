# include "Process.h"
# include <string>
using namespace std;

Process::Process(int at, int tc, int cb, int io, string stat)
    : AT(at), TC(tc), CB(cb), IO(io), state(stat)
{}

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

string Process::getState( void ) {
   return state;
}

