# include "Scheduler.h"
# include "util.h"
# include "Process.h"
# include <deque>

using namespace std;
using namespace util;

void FCFS::add_process(Process* process)
{
    run_queue.push_back(process);
}

Process* FCFS::get_next_process()
{
    Process *p = nullptr;
    if (!run_queue.empty())
    {
        p = run_queue.front();
        run_queue.pop_front();
    }
    return p;
}

void LCFS::add_process(Process *process) {
    run_queue.insert(run_queue.begin(), process);
}

Process* LCFS::get_next_process() {
    Process *process = nullptr;
    if (!run_queue.empty()) {
        process = run_queue.front();
        run_queue.pop_front();
    }
    return process;
}


void SJF::add_process(Process *process) {
    run_queue.push_back(process);
}

Process* SJF::get_next_process() {
    Process *process = nullptr;
    if (!run_queue.empty()) {
        int min = run_queue[0]->rem_CPU_time;
        int min_index = 0;
        for (int i=0; i < run_queue.size(); i++) {
            if (min > run_queue[i]->rem_CPU_time) {
                min = run_queue[i]->rem_CPU_time;
                min_index = i;
            }
        }
        process = run_queue[min_index];
        run_queue.erase(run_queue.begin() + min_index);
    }
    return process;
}

//RR
void RR::add_process(Process *p) {
    run_queue.push_back(p);
}

Process* RR::get_next_process() {
    Process *p = nullptr;
    if (!run_queue.empty()) {
        p = run_queue.front();
        run_queue.pop_front();
    }
    return p;
}

void PRIO::add_process(Process *p) {
    if (p->dynamic_priority == -1) {// the dynamic priority has reached -1, and p should be reset to its staic_priority
        // and put it to the expired queue
        p->dynamic_priority = p->static_priority - 1;
        switch (p->dynamic_priority) {
            case 0: {
                eq0.push_back(p);
                break;
            }
            case 1: {
                eq1.push_back(p);
                break;
            }
            case 2: {
                eq2.push_back(p);
                break;
            }
            case 3: {
                eq3.push_back(p);
                break;
            }
            default:
                break;
        }
    }
    else { // the dynamic priority hasn't reached -1, add it into the active queue.
        switch (p->dynamic_priority) {
            case 0: {
                aq0.push_back(p);
                break;
            }
            case 1: {
                aq1.push_back(p);
                break;
            }
            case 2: {
                aq2.push_back(p);
                break;
            }
            case 3: {
                aq3.push_back(p);
                break;
            }
            default:
                break;
        }
    }

}
bool PRIO::isAllEmpty() {
    return (aq0.empty() && aq1.empty() && aq2.empty() && aq3.empty()
            && eq0.empty() && eq1.empty() && eq2.empty() && eq3.empty());
}

bool PRIO::isActiveQueueEmpty() {
    return (aq0.empty() && aq1.empty() && aq2.empty() && aq3.empty());
}
Process* PRIO::get_next_process() {
    Process* p = nullptr;
    if (!isAllEmpty()) {
        if (isActiveQueueEmpty()) {
            aq0.swap(eq0);
            aq1.swap(eq1);
            aq2.swap(eq2);
            aq3.swap(eq3);
        }
        if (!aq3.empty()) {
            p = aq3.front();
            aq3.erase(aq3.begin());
        }
        else if (!aq2.empty()) {
            p = aq2.front();
            aq2.erase(aq2.begin());
        }
        else if (!aq1.empty()) {
            p = aq1.front();
            aq1.erase(aq1.begin());
        }
        else if (!aq0.empty()) {
            p = aq0.front();
            aq0.erase(aq0.begin());
        }
    }
    return p;
}
