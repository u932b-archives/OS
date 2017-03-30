# include "Scheduler.h"
# include "util.hpp"
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
    // Process *process = nullptr;
    Process *process = 0;
    if (!run_queue.empty())
    {
        process = run_queue.front();
        run_queue.pop_front();
    }
    return process;
}

void LCFS::add_process(Process *process) {
    run_queue.insert(run_queue.begin(), process);
}

Process* LCFS::get_next_process() {
    // Process *process = nullptr;
    Process *process = 0;
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
    Process *process = 0;
    // Process *process = nullptr;
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
void RR::add_process(Process *process) {
    run_queue.push_back(process);
}

Process* RR::get_next_process() {
    Process *process = 0;
    // Process *process = nullptr;
    if (!run_queue.empty()) {
        process = run_queue.front();
        run_queue.pop_front();
    }
    return process;
}

void PRIO::add_process(Process *process) {
    if (process->dynamic_priority == -1) {
        process->dynamic_priority = process->static_priority - 1;
        switch (process->dynamic_priority) {
            case 0: {
                equeue0.push_back(process);
                break;
            }
            case 1: {
                equeue1.push_back(process);
                break;
            }
            case 2: {
                equeue2.push_back(process);
                break;
            }
            case 3: {
                equeue3.push_back(process);
                break;
            }
            default:
                break;
        }
    }
    else {
        switch (process->dynamic_priority) {
            case 0: {
                aqueue0.push_back(process);
                break;
            }
            case 1: {
                aqueue1.push_back(process);
                break;
            }
            case 2: {
                aqueue2.push_back(process);
                break;
            }
            case 3: {
                aqueue3.push_back(process);
                break;
            }
            default:
                break;
        }
    }

}
bool PRIO::isAllEmpty() {
    return (aqueue0.empty() && aqueue1.empty() && aqueue2.empty() && aqueue3.empty()
            && equeue0.empty() && equeue1.empty() && equeue2.empty() && equeue3.empty());
}

bool PRIO::isActiveQueueEmpty() {
    return (aqueue0.empty() && aqueue1.empty() && aqueue2.empty() && aqueue3.empty());
}
Process* PRIO::get_next_process() {
    // Process* process = nullptr;
    Process* process = 0;
    if (!isAllEmpty()) {
        if (isActiveQueueEmpty()) {
            aqueue0.swap(equeue0);
            aqueue1.swap(equeue1);
            aqueue2.swap(equeue2);
            aqueue3.swap(equeue3);
        }
        if (!aqueue3.empty()) {
            process = aqueue3.front();
            aqueue3.pop_front();
        }
        else if (!aqueue2.empty()) {
            process = aqueue2.front();
            aqueue2.pop_front();
        }
        else if (!aqueue1.empty()) {
            process = aqueue1.front();
            aqueue1.pop_front();
        }
        else if (!aqueue0.empty()) {
            process = aqueue0.front();
            aqueue0.pop_front();
        }
    }
    return process;
}
