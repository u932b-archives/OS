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
Process* CURRENT_RUNNING_PROCESS = nullptr;
int CURRENT_TIME = 0;
vector <Process*> processes;
// rand
int ofs = 0;
int myrandom(int burst)
{
    ofs = ofs % randvals.size();
    int to_return = 1 + (randvals[ofs] % burst);
    cout << to_return << endl;
    ofs++;
    return to_return;
}
Event* get_event(deque<Event*>& event_queue)
{
    if (event_queue.empty())
    {
        return nullptr;
    }
    else
    {
        Event* return_event = event_queue.front();
        event_queue.pop_front();
        return return_event;
    }
}

void put_event(deque<Event*>& event_queue, Event* newEvent)
{
    int i = 0;
    while (i < event_queue.size() && newEvent->get_ts() >= event_queue[i]->get_ts())
    {
        i++;
    }
    event_queue.insert(event_queue.begin()+i, newEvent);
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

void Simulation(Scheduler* scheduler, deque<Event* >& event_queue, int quatum)
{
    int CALL_SCHEDULER = false;
    Event* evt;
    while ( (evt = get_event(event_queue)) )
    {
        Process* curr_process = processes[evt->pID];
        CURRENT_TIME = evt->TimeStamp;
        switch(evt->transition)
        {
            case TRANS_STATE::TRANS_TO_READY:
                if (curr_process->curr_state == STATE::CREATED)
                {//Number 1
                }
                else if (curr_process->curr_state == STATE::RUNNING)
                { //Number 5

                }
                else if (curr_process->curr_state == STATE::BLOCK)
                { // Number 4

                }
                scheduler->add_process(curr_process);
                CALL_SCHEDULER = true;
                break;
            case TRANS_STATE::TRANS_TO_RUN:
                // Number 2
                int curr_burst;
                if (curr_process->rem_burst>0)
                {
                    curr_burst = curr_process->rem_burst;
                }
                else
                {
                    curr_burst = myrandom(curr_process->getCB());
                }

                if (curr_process->rem_CPU_time <= curr_burst)
                {
                    // This process is gonna finish
                    curr_process->FT = CURRENT_TIME + curr_process->rem_CPU_time;
                    curr_process->rem_CPU_time = 0;
                    curr_process->FT = CURRENT_TIME;
                    CURRENT_RUNNING_PROCESS = nullptr;
                    CALL_SCHEDULER = true;
                    /*
                    Event newEvent(curr_process->getPID(), CURRENT_TIME,
                        TRANS_TO_DONE, RUNNING);
                    put_event(event_queue, &newEvent);
                    */
                }
                else
                {
                    // Number 3, going to IO
                    if (curr_burst <= quatum)
                    {
                        curr_process->rem_CPU_time -= curr_burst;
                        curr_process->rem_burst = 0;
                        Event newEvent(curr_process->getPID(), CURRENT_TIME,
                            TRANS_STATE::TRANS_TO_BLOCK, STATE::RUNNING);
                        put_event(event_queue, &newEvent);
                    }
                    else
                    // Number 5 on the state diagram
                    {
                        curr_process->rem_burst = curr_burst - quatum;
                        curr_process->rem_CPU_time -= quatum;
                        Event newEvent(curr_process->getPID(), CURRENT_TIME,
                            TRANS_STATE::TRANS_TO_READY, STATE::RUNNING);
                        put_event(event_queue, &newEvent);
                    }
                }
                break;
            case TRANS_STATE::TRANS_TO_BLOCK:
                {
                CURRENT_RUNNING_PROCESS = nullptr;
                int curr_io;
                curr_io = myrandom(curr_process->getIO());
                curr_process->IT += curr_io;
                Event newEvent(curr_process->getPID(), CURRENT_TIME,
                    TRANS_STATE::TRANS_TO_READY, STATE::BLOCK);
                put_event(event_queue, &newEvent);
                CALL_SCHEDULER = true;
                break;
                }
            case TRANS_STATE::TRANS_TO_DONE:
                break;
            default:
                break;
        }
        delete evt;
        if (CALL_SCHEDULER)
        {
            if (get_next_event_time(event_queue) == CURRENT_TIME)
            {
                continue;
            }
            CALL_SCHEDULER = false;
            if (CURRENT_RUNNING_PROCESS == nullptr)
            {
                CURRENT_RUNNING_PROCESS = scheduler->get_next_process();
                if (CURRENT_RUNNING_PROCESS == nullptr)
                {
                    continue;
                }
                Event newEvent(CURRENT_RUNNING_PROCESS->getPID(), CURRENT_TIME,
                        TRANS_STATE::TRANS_TO_RUN, STATE::READY);
                put_event(event_queue, &newEvent);
                CURRENT_RUNNING_PROCESS = nullptr;
            }
        }

    }
}

void result()
{
    for (int i=0; i < processes.size(); i++)
    {
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
                processes[i]->getPID(), processes[i]->getAT(), processes[i]->getTC(),
                processes[i]->getCB(), processes[i]->getIO(), processes[i]->static_priority,
                processes[i]->FT, processes[i]->TT, processes[i]->IT, processes[i]->CW);
    }
}


int main(int argc, char *argv[])
{
	if (argc > 1) {
      // This will print the first argument passed to your program
    }
    char *svalue = NULL;
    int c;
	opterr = 0;
	int index;

    while ((c = getopt (argc, argv, "vs:")) != -1)
    {
        switch (c)
        {
            case 'v':
                cout << "got v" << endl;
                break;
            /*
            case 's':
                cout << "got s" << endl;
                svalue = optarg;
                break;
            */
            /*
			case '?':
                cout << "got ?" << endl;
       			if (optopt == 'c')
       			   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
       			else if (isprint (optopt))
       			  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
       			else
       			  fprintf (stderr,
       			           "Unknown option character `\\x%x'.\n",
       			           optopt);
       			return -1;
            */
            default:
                abort ();

        }
    }
	ifstream inFile;
    ifstream randFile;
    // cout << argv[optind] << endl;
	inFile.open(argv[optind]);
	randFile.open(argv[optind+1]);
	string line;
    deque <Event* > event_queue;
	// vector <Process *> DES;
    int quatum = 10000;
    string get_s;

    // get random
    string rand_line;
    while(!randFile.eof())
    {
        getline(randFile, rand_line);
        // cout << rand_line << endl;
        randvals.push_back(atoi(rand_line.c_str()));
    }
    cout << randvals.size() << endl;
    randFile.close();

    // create processes
    while (!inFile.eof())
    {
        getline(inFile, line);
        if (!line.empty())
        {
            cout << "Confirmation of data reading: " << line << endl;
			string buf; // Have a buffer string
    		stringstream ss(line); // Insert the string into a stream
    		vector<string> tokens; // Create vector to hold our words
            int pid_counter=0;
    		while (ss >> buf)
        		tokens.push_back(buf);
            STATE state = STATE::CREATED;
            cout << "Here" << endl;
            cout << myrandom(4) << endl;
            Process newProcess(pid_counter,
                    stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2])
			        , stoi(tokens[3]), state, myrandom(4));
			 processes.push_back(&newProcess);
             pid_counter++;
        }
    }
	inFile.close();

    // initialize DES
    cout << "Here" << endl;
	for (int i=0; i < processes.size(); i++)
	{
        Event newEvent(i, processes[i]->getAT(), TRANS_STATE::TRANS_TO_READY,
                STATE::CREATED);
        event_queue.push_back(&newEvent);
	}


    get_s="F";
    char sched_type=get_s.at(0);
    Scheduler* scheduler;
    switch(sched_type)
    {
        case 'F':
            scheduler = new FCFS();
            break;
        default:
            cout << "No such scheduler" << endl;
            exit (EXIT_FAILURE);
            break;
    }

    Simulation(scheduler, event_queue, quatum);
    result();

}
