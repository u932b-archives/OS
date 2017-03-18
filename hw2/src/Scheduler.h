#ifndef SCHEDULER_H
#define SCHEDULER_H

# include "Process.h"
# include "Event.h"
# include <deque>
using namespace std;

class Scheduler
{
    public:
        deque<Process* > run_queue;
        virtual Process* get_next_process() = 0;
        virtual void add_process(Process* process) = 0;

        //void set_ready_queue();
        //Event* run_this_process(Process* process);
};

class FCFS : public Scheduler
{
    public:
        // Event* get_event();
        void add_process(Process* process);
        Process* get_next_process();
};

#endif //SCHEDULER_H
