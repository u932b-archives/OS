#ifndef SCHEDULER_H
#define SCHEDULER_H

# include "Process.h"
# include "Event.h"
# include <deque>
# include <vector>
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

class LCFS : public Scheduler {
public:
    void add_process(Process *p);
    Process *get_next_process();
};


class SJF : public Scheduler {
    public:
        void add_process(Process *p);
        Process *get_next_process();
};

class RR : public Scheduler {
    public:
    void add_process(Process *p);

    Process *get_next_process();
};

class PRIO : public Scheduler {
public:
    // active queue
    deque<Process*> aqueue0;
    deque<Process*> aqueue1;
    deque<Process*> aqueue2;
    deque<Process*> aqueue3;

    // expried queue
    deque<Process*> equeue0;
    deque<Process*> equeue1;
    deque<Process*> equeue2;
    deque<Process*> equeue3;

    void add_process(Process *p);

    Process *get_next_process();

    bool isAllEmpty();

    bool isActiveQueueEmpty();
};

#endif //SCHEDULER_H
