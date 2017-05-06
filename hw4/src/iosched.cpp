# include <iostream>
# include <fstream>
# include <unistd.h>
# include <vector>
# include <sstream>
# include <iomanip>
#include <bitset>

using namespace std;

struct pte
{
    int PRESENT:1;
    int MODIFIED:1;
    int REFERENCED:1;
    int PAGEDOUT:1;
    int idx:8;
    int frame_index:8;
    bitset<32> age_bit_vector;
};


class io_op
{
    public:
        int index;
        int time;
        int track_nmbr;
        int start_time;
        int end_time;
        bool io_processing= false;
        io_op(int, int, int);
};

class STAT
{
    public:
        int total_time;
        int tot_movement;
        float avg_turnaround;
        float avg_waittime;
        int max_waittime;
        STAT(int, int, float, float, int);
};

io_op::io_op(int idx, int t, int track_n)
{
    index = idx;
    time = t;
    track_nmbr = track_n;
}

vector<io_op> event_queue;

class base_class {
};

class FIFO : public base_class{
    public:
};

STAT stats = {0,0,0,0,0};
vector<io_op*> to_add;
vector<io_op> to_issue;
int curr_time = 1;
bool io_in_process = false;
int curr_track = 0;
bool go_right = false;

void ADD()
{
    cout << curr_time << ":" << setw(5) << event_queue[0].index << " add " <<
        event_queue[0].track_nmbr << endl;
    event_queue[0].start_time = curr_time;
    to_issue.push_back(event_queue[0]);
    event_queue.erase(event_queue.begin());
}
void ISSUE()
{
    cout << curr_time << ":" << setw(5) << to_issue[0].index << " issue " <<
    to_issue[0].track_nmbr << " " << curr_track << endl;
    if (to_issue[0].track_nmbr > curr_track)
    {
        go_right = true;
    }
    else
    {
        go_right = false;
    }
    io_in_process = true;
}
void FINISH()
{
    cout << curr_time << ":" << setw(5) << to_issue[0].index << " finish " <<
            curr_time - to_issue[0].start_time << endl;
    to_issue.erase(to_issue.begin());
    io_in_process = false;
}

void PRINTSUM(int total_time, int tot_movement, float avg_turnaround, float avg_waittime, int max_waittime)
{
    printf("SUM: %d %d %.2lf %.2lf %d\n", total_time,
    tot_movement, avg_turnaround, avg_waittime, max_waittime);

}

void IOREQS()
{

}

int main(int argc, char *argv[])
{
	if (argc < 3) {
        cout << "Not enough arguments" << endl;
        return -1;
    }
    char *svalue = NULL;
    int c;
	opterr = 0;
    string alg;
    string tmp;

    while ((c = getopt (argc, argv, "s:")) != -1)
    {
        switch (c)
        {
            case 's':
                alg = optarg;
                break;
            default:
                abort ();
        }
    }
	ifstream inFile;
	inFile.open(argv[optind]);
	string line;


    int curr_idx = 0;
    while (!inFile.eof())
    {
        getline(inFile, line);
        bool break_flag = false;
        if (!line.empty())
        {
		    string buf; // Have a buffer string
    	    stringstream ss(line); // Insert the string into a stream
    	    vector<string> tokens; // Create vector to hold words
    	    while (ss >> buf)
            {
                if (break_flag == false)
                {
                    if (buf[0] == '#')
                    {
                        break_flag = true;
                    }
                    else
                    {
            	        tokens.push_back(buf);
                    }
                }
            }
            if (break_flag == false)
            {
                io_op curr_op = io_op(curr_idx, stoi(tokens[0]), stoi(tokens[1]));
                curr_idx++;
                event_queue.push_back(curr_op);
            }
        }
    }

    for (int i = 0; i< event_queue.size(); i++)
    {
        to_add.push_back(&event_queue[i]);
    }

    cout << "TRACE" << endl;
    while (!event_queue.empty())
    {
        bool did_sth = false;
        if (event_queue[0].time == curr_time)
        {
            did_sth = true;
            ADD();
        }

        if (!io_in_process)
        {
            did_sth = true;
            ISSUE();
        }
        else
        {
            // check if current io can be finished
            if (to_issue[0].track_nmbr == curr_track)
            {
                did_sth = true;
                FINISH();
            }
        }

        if (!did_sth)
        {
            if (go_right==true)
            {
                curr_track++;
            }
            else
            {
                curr_track--;
            }
            curr_time++;
        }
    }
    while (!to_issue.empty())
    {
        bool did_sth = false;
        if (!io_in_process)
        {
            did_sth = true;
            ISSUE();
        }
        else
        {
            // check if current io can be finished
            if (to_issue[0].track_nmbr == curr_track)
            {
                did_sth = true;
                FINISH();
            }
        }

        if (!did_sth)
        {
            if (go_right==true)
            {
                curr_track++;
            }
            else
            {
                curr_track--;
            }
            curr_time++;
        }
    }

    /*
    if (print_flag)
    {
        print_pagetable();
    }
    if (frametable_flag)
    {
        debug_frametable_flag = false;
        print_frametable(alg, debug_frametable_flag);
    }
    if (sum_flag)
    {
        print_sum(stat);
    }
    */
	inFile.close();
}
