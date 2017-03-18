# include "Scheduler.h"
# include "util.h"
# include "Process.h"
# include "deque"

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

