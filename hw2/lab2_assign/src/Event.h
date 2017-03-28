# ifndef EVENT_H
# define EVENT_H

# include "util.hpp"
using namespace util;

class Event
{
    public:
        Event(int, int, TRANS_STATE, STATE);
        void setState(TRANS_STATE, STATE);
        int get_ts();

    TRANS_STATE transition;
    STATE cur_state;
    int pID;
    int TimeStamp;
    int last_burst_duration;
    int curr_burst_duration;
    int rem_cpu_time;
};

#endif /*EVENT_H*/
