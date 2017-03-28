# include "Event.h"
// # include "util.hpp"
using namespace util;

Event::Event(int pid, int timestamp, TRANS_STATE t_state, STATE cur)
    :pID(pid), TimeStamp(timestamp), transition(t_state), cur_state(cur)
{}

void Event::setState(TRANS_STATE t_state, STATE cur)
{
    transition = t_state;
    cur_state = cur;
}

int Event::get_ts()
{
    return TimeStamp;
}

