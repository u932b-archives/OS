# include <iostream>
# include <fstream>
# include <unistd.h>
# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <string>
# include <sstream>
# include <vector>
# include <iterator>
# include <deque>

# include "Process.h"
# include "Event.h"
# include "Scheduler.h"
# include "util.hpp"

using namespace std;
using namespace util;

vector <int> randvals;
int CURRENT_TIME = 0;
int finish_time = 0;
int io_time = 0;
int last_io_end_time = 0;
vector <Process*> processes;
// rand
int ofs = 0;
int myrandom(int burst)
{
    ofs = ofs % randvals.size();
    int to_return = 1 + (randvals[ofs] % burst);
    ofs++;
    // cout << "random: " << to_return << endl;
    return to_return;
}
Event* get_event(deque<Event*>& event_queue)
{
    if (event_queue.empty())
    {
        // return nullptr;
        return 0;
    }
    else
    {
        Event* return_event = event_queue.front();
        event_queue.pop_front();
        return return_event;
    }
}

void event_checker(deque<Event*>& event_queue) //, TRANS_STATE trans = NULL)
{
	cout << "Event_Queue: ";
	for (int i=0; i < event_queue.size(); i++)
	{
		cout << event_queue[i]->pID << " ";
	}
	cout << endl;
}

void put_event(deque<Event*>& event_queue, Event* newEvent)
{
    int i = 0;
	// put into the correct spot
    while (i < event_queue.size() && newEvent->get_ts() >= event_queue[i]->get_ts())
    {
        i++;
    }
	// event_checker(event_queue);
	// cout << "put "<< newEvent->pID << " in at location " << i << " in the event queue" << endl;
    event_queue.insert(event_queue.begin()+i, newEvent);
	// event_checker(event_queue);
}

int get_next_event_time(deque<Event*> event_queue)
{
    if (event_queue.empty())
    {
        return -1;
    }
    else
    {
        return event_queue.front()->TimeStamp;
    }
}

void Simulation(Scheduler* scheduler, deque<Event* >& event_queue, int quantum)
{
    Process* CURRENT_RUNNING_PROCESS = 0;
    // Process* CURRENT_RUNNING_PROCESS = nullptr;
    int CALL_SCHEDULER = false;
    Event* evt;
    while ( (evt = get_event(event_queue)) )
    {
		if (evt->TimeStamp == -1)
			break;
        Process* curr_process = processes[evt->pID];
		// cout << "process in simulation:" << curr_process->pId << " remaining cpu time:" << curr_process->rem_CPU_time <<endl;
        CURRENT_TIME = evt->TimeStamp;
        curr_process->curr_state = evt->cur_state;
        // cout << "CURRENT_TIME: " << CURRENT_TIME << endl;
        switch(evt->transition)
        {
            case TRANS_STATE::TRANS_TO_READY:
				// cout << "TRANS_TO_READY" << endl;
                if (curr_process -> curr_state == STATE::CREATED)
                {//Number 1
				    curr_process->enter_ready_time = CURRENT_TIME;
                }
                else if (curr_process -> curr_state == STATE::RUNNING)
                { //Number 5
                    CURRENT_RUNNING_PROCESS = 0;
                    // CURRENT_RUNNING_PROCESS = nullptr;
                    curr_process->dynamic_priority -= 1;
                }
                else if (curr_process -> curr_state == STATE::BLOCK)
                { // Number 4
					processes[curr_process->pId]->dynamic_priority =
					processes[curr_process->pId]->static_priority-1;
                }
				curr_process->enter_ready_time = CURRENT_TIME;
                scheduler->add_process(curr_process);
                CALL_SCHEDULER = true;
                break;
            case TRANS_STATE::TRANS_TO_RUN:
                // Number 2
				// cout << "TRANS_TO_RUN" << endl;
                CURRENT_RUNNING_PROCESS = curr_process;
				curr_process->CW += CURRENT_TIME - curr_process->enter_ready_time ;

                int curr_burst;
                if (curr_process->rem_burst>0)
                {
                    curr_burst = curr_process->rem_burst;
                }
                else
                {
                    curr_burst = myrandom(curr_process->getCB());
                    // cout << "random is " << curr_burst << endl;
                }

                if (curr_process->rem_CPU_time <= curr_burst)
                {
                    // This process is gonna finish
                    Event* newEvent = new Event(curr_process->getPID(),
                            CURRENT_TIME + curr_process-> rem_CPU_time,
                            TRANS_STATE::TRANS_TO_DONE, STATE::RUNNING);
                    put_event(event_queue, newEvent);

                    curr_process->rem_CPU_time = -1;
                }
                else
                {
                    // Number 3, going to IO
                    if (curr_burst <= quantum)
                    {
                        curr_process->rem_CPU_time -= curr_burst;
                        curr_process->rem_burst = -1;
                        Event* newEvent = new Event(curr_process->getPID(),
                                CURRENT_TIME+curr_burst,
                                TRANS_STATE::TRANS_TO_BLOCK, STATE::RUNNING);
                        put_event(event_queue, newEvent);
                    }
                    else
                    // Number 5 on the state diagram, Preemption
                    {

                        curr_process->rem_burst = curr_burst - quantum;
                        curr_process->rem_CPU_time -= quantum;

                        Event* newEvent = new Event(curr_process->getPID(),
                                CURRENT_TIME+quantum, TRANS_STATE::TRANS_TO_READY,
                                STATE::RUNNING);
                        put_event(event_queue, newEvent);

                    }
                }
                break;
            case TRANS_STATE::TRANS_TO_BLOCK:
                {
				// cout << "TRANS_TO_BLOCK" << endl;
                CURRENT_RUNNING_PROCESS = 0;
                // CURRENT_RUNNING_PROCESS = nullptr;
                int curr_io;
                curr_io = myrandom(curr_process->getIO());
                    // cout << "random is " << curr_io << endl;
		// cout << "adding IO" << endl;
                curr_process->IT += curr_io;
                Event* newEvent = new Event(curr_process->getPID(), CURRENT_TIME + curr_io,
                    TRANS_STATE::TRANS_TO_READY, STATE::BLOCK);
                put_event(event_queue, newEvent);
				// event_checker(event_queue, evt->transition);
				if (CURRENT_TIME >= last_io_end_time)
                {
                    io_time += curr_io;
                    last_io_end_time = CURRENT_TIME + curr_io;
                }
                else if (CURRENT_TIME + curr_io > last_io_end_time)
                {
                        io_time += (CURRENT_TIME + curr_io - last_io_end_time);
                        last_io_end_time = CURRENT_TIME + curr_io;
                }
                // cout << "CURRENT_TIME when leaving block: " << CURRENT_TIME << endl;
                CALL_SCHEDULER = true;
                break;
                }

            case TRANS_STATE::TRANS_TO_DONE:
                // CURRENT_TIME = CURRENT_TIME + curr_burst;
                // cout << "TRANS_TO_DONE" << endl;
                // curr_process->FT = CURRENT_TIME;
                curr_process->FT = evt->TimeStamp;
                CURRENT_RUNNING_PROCESS = 0;
                // CURRENT_RUNNING_PROCESS = nullptr;
				// finish_time = CURRENT_TIME;
				finish_time = evt->TimeStamp;
                CALL_SCHEDULER = true;
                break;

            default:
                break;
        }
		if (evt)
        {
			// cout << evt->pID << endl;
		}
        if (CALL_SCHEDULER)
        {
            // cout << "CALLING SCHEDULER" << endl;
            if (get_next_event_time(event_queue) == CURRENT_TIME)
            {
                continue;
            }
            CALL_SCHEDULER = false;
            // if (CURRENT_RUNNING_PROCESS == nullptr)
            if (CURRENT_RUNNING_PROCESS == 0)
            {
		// cout << "get next process" << endl;
                CURRENT_RUNNING_PROCESS = scheduler->get_next_process();
                // if (CURRENT_RUNNING_PROCESS == nullptr)
                if (CURRENT_RUNNING_PROCESS == 0)
                {
                    // cout << "no running process" << endl;
                    continue;
                }
                else
                {
                    Event newEvent(CURRENT_RUNNING_PROCESS->getPID(), CURRENT_TIME,
                            TRANS_STATE::TRANS_TO_RUN, STATE::READY);
                    put_event(event_queue, &newEvent);
                    CURRENT_RUNNING_PROCESS = 0;
                    // CURRENT_RUNNING_PROCESS = nullptr;
                }
            }
        }

    }
}

void result(string sched_name, int num)
{
	int cpu_time = 0; // total cpu time
    int total_TT = 0;
    int total_CW = 0;
    if (num > 0)
    {
        printf("%s %d\n", sched_name.c_str(), num);
    }
    else
    {
        printf("%s\n", sched_name.c_str());
    }
    for (int i=0; i < processes.size(); i++)
    {
	cpu_time += processes[i]->getTC();
        processes[i]->TT = processes[i]->FT - processes[i]->getAT();
        total_TT += processes[i]->TT;
        total_CW += processes[i]->CW;
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
                processes[i]->getPID(), processes[i]->getAT(), processes[i]->getTC(),
                processes[i]->getCB(), processes[i]->getIO(), processes[i]->static_priority,
                processes[i]->FT, processes[i]->TT, processes[i]->IT, processes[i]->CW);
    }

	double cpu_util = 100 * ((double) cpu_time / (double) finish_time);
    double io_util = 100 * ((double) io_time / (double) finish_time);
    double average_TT = (double) total_TT / (double) processes.size();
    double average_CW = (double) total_CW / (double) processes.size();
    double throughput_number = (double) processes.size() * 100 / (double) finish_time;

    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
       finish_time, cpu_util, io_util, average_TT, average_CW, throughput_number);
}


int main(int argc, char *argv[])
{
	if (argc > 1) {
      // This will print the first argument passed to your program
    }
    char *svalue = NULL;
    int c;
	opterr = 0;
    string param;

    while ((c = getopt (argc, argv, "vs:")) != -1)
    {
        switch (c)
        {
            case 'v':
                // cout << "verbose" << endl;
                break;
            case 's':
                param = optarg;
                break;
            default:
                abort ();
        }
    }
	ifstream inFile;
    ifstream randFile;
	inFile.open(argv[optind]);
	randFile.open(argv[optind+1]);
	string line;
    deque <Event* > event_queue;
    int quantum = 10000;

    // get random
    int randfile_count=0;
    string first_line;
    getline(randFile, first_line);
    randfile_count = stoi(first_line);
    string rand_line;
    while(!randFile.eof())
    {
        getline(randFile, rand_line);
        if (!rand_line.empty())
        {
            randvals.push_back(stoi(rand_line));
        }
    }
    randFile.close();

    // create processes
    int pid_counter=0;
    while (!inFile.eof())
    {
        getline(inFile, line);
        if (!line.empty())
        {
            // cout << "Confirmation of data reading: " << line << endl;
			string buf; // Have a buffer string
    		stringstream ss(line); // Insert the string into a stream
    		vector<string> tokens; // Create vector to hold words
    		while (ss >> buf)
        		tokens.push_back(buf);
            STATE state = STATE::CREATED;
            Process *newProcess = new Process(pid_counter,
                    stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2])
			        , stoi(tokens[3]), state, myrandom(4));
		    processes.push_back(newProcess);
            pid_counter++;
        }
    }
	inFile.close();

    // initialize DES
	for (int i=0; i < processes.size(); i++)
	{
        Event* newEvent = new Event(i, processes[i]->getAT(),
									TRANS_STATE::TRANS_TO_READY,
                					STATE::CREATED);
                                    event_queue.push_back(newEvent);
	}

    char sched_type = param.at(0);
    Scheduler* scheduler;
    string sched_name;
    int num;
    switch(sched_type)
    {
        case 'F':
            sched_name = "FCFS";
            scheduler = new FCFS();
            break;
        case 'L':
            sched_name = "LCFS";
            scheduler = new LCFS();
            break;
        case 'S':
            sched_name = "SJF";
            scheduler = new SJF();
            break;
        case 'R':
            sched_name = "RR";
            quantum = stoi(param.substr(1));
            scheduler = new RR();
            break;
        case 'P':
            sched_name = "PRIO";
            quantum = stoi(param.substr(1));
            scheduler = new PRIO();
            break;

        default:
            cout << "No such scheduler" << endl;
            exit (EXIT_FAILURE);
            break;
    }

    Simulation(scheduler, event_queue, quantum);
    result(sched_name, num=0);

}
