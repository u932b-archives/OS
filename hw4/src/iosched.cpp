# include <iostream>
# include <fstream>
# include <unistd.h>
# include <vector>
# include <sstream>
# include <iomanip>
#include <bitset>
# include <climits>
#include <cmath>

using namespace std;

class io_op
{
    public:
        int index;
        int time;
        int track_nmbr;
        int added_time;
        int start_time;
        int end_time;
        bool io_processing;
        io_op(int, int, int);
};

io_op::io_op(int idx, int t, int track_n)
{
    index = idx;
    time = t;
    track_nmbr = track_n;
}

vector<io_op> event_queue;
vector<io_op*> to_add;
vector<io_op*> to_issue;
int curr_time = 1;
int muvement = 0;
bool io_in_process = false;
int curr_track = 0;
bool go_right = false;
bool verbose = true;
int numio;
int maxtracks;
string lambda;
int issuing_index = 0;

class Scheduler
{
    public:
        virtual void set_index(int i) {
                 issuing_index = i;
              }
        int get_index() {
                 return issuing_index;
              }
        virtual void ADD() = 0;
        virtual void ISSUE() = 0;
        virtual void FINISH() = 0;
    // pirivate:
        // int issuing_index = 0;
};

class FIFO : public Scheduler
{
//class FIFO {
    public:
        // vector<io_op*> to_add;
        // vector<io_op*> to_issue;
        // int issuing_index = 0;
        void ADD()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_add[0]->index << " add " <<
                    to_add[0]->track_nmbr << endl;
            }
            to_add[0]->added_time = curr_time;
            to_issue.push_back(to_add[0]);
            to_add.erase(to_add.begin());
        }
        void ISSUE()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[0]->index << " issue " <<
                to_issue[0]->track_nmbr << " " << curr_track << endl;
            }
            to_issue[0]->start_time = curr_time;
            if (to_issue[0]->track_nmbr > curr_track)
            {
                go_right = true;
            }
            else if (to_issue[0]->track_nmbr < curr_track)
            {
                go_right = false;
            }
            else
            {
                // go_right stay the same
            }
            io_in_process = true;
        }
        void FINISH()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[0]->index << " finish " <<
                    curr_time - to_issue[0]->added_time << endl;
            }
            to_issue[0]->end_time = curr_time;
            to_issue.erase(to_issue.begin());
            io_in_process = false;
        }
};

class SCAN : public Scheduler
{
    public:
        // vector<io_op*> to_add;
        // vector<io_op*> to_issue;
        // int issuing_index = -1;

        void ADD()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_add[0]->index << " add " <<
                    to_add[0]->track_nmbr << endl;
            }
            to_add[0]->added_time = curr_time;
            to_issue.push_back(to_add[0]);
            to_add.erase(to_add.begin());
        }
        void ISSUE()
        {
            int delta = INT_MAX;
            int index_to_issue = -1;
            while (index_to_issue == -1)
            {
                for (int i =0; i< to_issue.size(); i++)
                {
                    int curr_delta = to_issue[i]->track_nmbr - curr_track;
                    if (go_right)
                    {
                        if (curr_delta < 0)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if (curr_delta > 0)
                        {
                            continue;
                        }
                    }
                    curr_delta = abs(curr_delta);
                    if (curr_delta < delta)
                    {
                        delta = curr_delta;
                        index_to_issue = i;
                    }
                }
                if (index_to_issue == -1)
                {
                    go_right = !go_right;
                }
            }
            to_issue[index_to_issue]->start_time = curr_time;
            if (to_issue[index_to_issue]->track_nmbr > curr_track)
            {
                go_right = true;
            }
            else if (to_issue[index_to_issue]->track_nmbr < curr_track)
            {
                go_right = false;
            }
            else
            {
                // go_right stay the same
            }

            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[index_to_issue]->index << " issue " <<
                to_issue[index_to_issue]->track_nmbr << " " << curr_track << endl;
            }
            issuing_index = index_to_issue;
            io_in_process = true;
        }
        void FINISH()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[issuing_index]->index << " finish " <<
                    curr_time - to_issue[issuing_index]->added_time << endl;
            }
            to_issue[issuing_index]->end_time = curr_time;
            to_issue.erase(to_issue.begin() + issuing_index);
            io_in_process = false;
        }
};

class CSCAN : public Scheduler
{
    public:
        // int issuing_index = -1;
        void ADD()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_add[0]->index << " add " <<
                    to_add[0]->track_nmbr << endl;
            }
            to_add[0]->added_time = curr_time;
            to_issue.push_back(to_add[0]);
            to_add.erase(to_add.begin());
        }
        void ISSUE()
        {
            int delta = INT_MAX;
            int curr_delta = INT_MAX;
            int index_to_issue = -1;
            go_right = true;
            bool circular = false;
            while (index_to_issue == -1)
            {
                for (int i = 0; i< to_issue.size(); i++)
                //for (int i =0; i< to_issue.size(); i++)
                {
                    if (!circular)
                    {
                        curr_delta = to_issue[i]->track_nmbr - curr_track;
                    }
                    else
                    {
                        curr_delta = (to_issue[i]->track_nmbr + maxtracks - curr_track);
                    }
                    if (curr_delta < 0)
                    {
                        continue;
                    }
                    if (curr_delta < delta)
                    {
                        delta = curr_delta;
                        index_to_issue = i;
                    }
                }
                if (index_to_issue == -1)
                {
                    circular = true;
                }
            }
            if (circular)
            {
                go_right = !go_right;
            }
            to_issue[index_to_issue]->start_time = curr_time;
            /*
            if (to_issue[index_to_issue]->track_nmbr > curr_track)
            {
                go_right = true;
            }
            else if (to_issue[index_to_issue]->track_nmbr < curr_track)
            {
                go_right = false;
            }
            else
            {
                // go_right stay the same
            }
            */

            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[index_to_issue]->index << " issue " <<
                to_issue[index_to_issue]->track_nmbr << " " << curr_track << endl;
            }
            issuing_index = index_to_issue;
            io_in_process = true;
        }
        void FINISH()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[issuing_index]->index << " finish " <<
                    curr_time - to_issue[issuing_index]->added_time << endl;
            }
            to_issue[issuing_index]->end_time = curr_time;
            to_issue.erase(to_issue.begin() + issuing_index);
            io_in_process = false;
        }
};

class SSTF : public Scheduler
{
    public:
        /*
        void ADD()
        {
            int delta = INT_MAX;
            int index_to_issue = 0;
            for (int i=0; i< to_issue.size(); i++)
            {
                int curr_delta = abs(to_issue[i]->track_nmbr - curr_track);
                if (curr_delta < delta)
                {
                    delta = curr_delta;
                    index_to_issue = i;
                }
            }
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_add[0]->index << " add " <<
                    to_add[0]->track_nmbr << endl;
            }
            to_add[0]->added_time = curr_time;
            cout << "insert to " << index_to_issue << endl;
            // to_issue.insert(to_issue.end() - index_to_issue + 1, to_add[0]);
            to_issue.insert(to_issue.begin() + index_to_issue, to_add[0]);
            //to_issue.push_back(to_add[index_to_issue]);
            to_add.erase(to_add.begin());
            // ato_add.erase (to_add.begin() + index_to_issue);
        }*/
        // int issuing_index = -1;
        // Scheduler::set_index(-1);
        // Scheduler.set_index(-1);
        void ADD()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_add[0]->index << " add " <<
                    to_add[0]->track_nmbr << endl;
            }
            to_add[0]->added_time = curr_time;
            to_issue.push_back(to_add[0]);
            to_add.erase(to_add.begin());
        }
        void ISSUE()
        {
            int delta = INT_MAX;
            int index_to_issue = 0;
            for (int i =0; i< to_issue.size(); i++)
            {
                int curr_delta = abs(to_issue[i]->track_nmbr - curr_track);
                if (curr_delta < delta)
                {
                    delta = curr_delta;
                    index_to_issue = i;
                }
            }
            to_issue[index_to_issue]->start_time = curr_time;
            if (to_issue[index_to_issue]->track_nmbr > curr_track)
            {
                go_right = true;
            }
            else if (to_issue[index_to_issue]->track_nmbr < curr_track)
            {
                go_right = false;
            }
            else
            {
                // go_right stay the same
            }
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[index_to_issue]->index << " issue " <<
                to_issue[index_to_issue]->track_nmbr << " " << curr_track << endl;
            }
            issuing_index = index_to_issue;
            io_in_process = true;
        }
        void FINISH()
        {
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue[issuing_index]->index << " finish " <<
                    curr_time - to_issue[issuing_index]->added_time << endl;
            }
            to_issue[issuing_index]->end_time = curr_time;
            to_issue.erase(to_issue.begin() + issuing_index);
            io_in_process = false;
        }

};

class FSCAN: public Scheduler
{
    vector<io_op*>* curr_to_issue;
    vector<io_op*> to_issue1;
    vector<io_op*> to_issue2;
    bool queue_to_add ;
    bool queue_to_issue;
    void ADD()
    {
        // cout << "one's size:" << to_issue1.size() << endl;
        if (verbose)
        {
            cout << curr_time << ":" << setw(6) << to_add[0]->index << " add " <<
                to_add[0]->track_nmbr << endl;
        }
        to_add[0]->added_time = curr_time;
        if (to_issue1.empty() && to_issue2.empty())
        {
            //reset
            queue_to_add = true;
            queue_to_issue = true;
        }
        if (queue_to_add)
        {
            curr_to_issue = &to_issue1;
        }
        else
        {
            curr_to_issue = &to_issue2;
        }
        curr_to_issue->push_back(to_add[0]);
        if (!io_in_process)
        {
            to_issue = *curr_to_issue;
        }
        to_add.erase(to_add.begin());
    }
    void ISSUE()
    {
        int delta = INT_MAX;
        int index_to_issue = -1;
        // to_issue = *curr_to_issue;
        if (queue_to_issue)
        //if (queue_in_io_is_first)
        {
            // copy back
            to_issue = to_issue1;
        }
        else
        {
            to_issue = to_issue2;
        }
        // cout << "one's size after issue:" << to_issue1.size() << endl;
        while (index_to_issue == -1)
        {
            for (int i =0; i< to_issue.size(); i++)
            {
                int curr_delta = to_issue[i]->track_nmbr - curr_track;
                if (go_right)
                {
                    if (curr_delta < 0)
                    {
                        continue;
                    }
                }
                else
                {
                    if (curr_delta > 0)
                    {
                        continue;
                    }
                }
                curr_delta = abs(curr_delta);
                if (curr_delta < delta)
                {
                    delta = curr_delta;
                    index_to_issue = i;
                }
            }
            if (index_to_issue == -1)
            {
                go_right = !go_right;
            }
        }
        to_issue[index_to_issue]->start_time = curr_time;
        if (to_issue[index_to_issue]->track_nmbr > curr_track)
        {
            go_right = true;
        }
        else if (to_issue[index_to_issue]->track_nmbr < curr_track)
        {
            go_right = false;
        }
        else
        {
            // go_right stay the same
        }

        if (verbose)
        {
            cout << curr_time << ":" << setw(6) << to_issue[index_to_issue]->index << " issue " <<
            to_issue[index_to_issue]->track_nmbr << " " << curr_track << endl;
        }
        if (queue_to_issue)
        {
            // copy back
            to_issue1 = to_issue;
        }
        else
        {
            to_issue2 = to_issue;
        }
        issuing_index = index_to_issue;
        io_in_process = true;
        queue_to_add = !queue_to_issue;
        // if (to_issue.size() == 0)
        // {
        //     queue_to_issue = !queue_to_issue;
        // }
    }

    void FINISH()
    {
        if (queue_to_issue)
        {
            curr_to_issue = &to_issue1;
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue1[issuing_index]->index << " finish " <<
                    curr_time - to_issue1[issuing_index]->added_time << endl;
            }
            to_issue1[issuing_index]->end_time = curr_time;
            to_issue1.erase(to_issue1.begin() + issuing_index);
            // to_issue = to_issue1;
            }
        else
        {
            curr_to_issue = &to_issue2;
            if (verbose)
            {
                cout << curr_time << ":" << setw(6) << to_issue2[issuing_index]->index << " finish " <<
                    curr_time - to_issue2[issuing_index]->added_time << endl;
            }
            to_issue2[issuing_index]->end_time = curr_time;
            to_issue2.erase(to_issue2.begin() + issuing_index);
            // to_issue = to_issue2;
        }
        // first_queue = !first_queue;
        if (curr_to_issue->size() == 0)
        {
            queue_to_issue = !queue_to_issue;
            queue_to_add = !queue_to_issue;
            go_right = true;
        }
        if (to_issue1.size() == 0 && to_issue2.size()==0)
        {
            to_issue = to_issue1;
        }
        io_in_process = false;
    }
};
void PRINTSUM(int total_time, int tot_movement)
{
    int tot_turnaround = 0;
    int tot_waittime = 0;
    int max_waittime=0;

    for (int i=0; i< event_queue.size(); i++)
    {
        io_op event = event_queue[i];
        tot_turnaround += event.end_time - event.added_time;
        int event_wt = event.start_time - event.added_time;
        tot_waittime += event_wt;
        if (event_wt > max_waittime)
        {
            max_waittime = event_wt;
        }
    }

    float avg_turnaround= static_cast<float>(tot_turnaround)/static_cast<float>(event_queue.size());
    //float avg_waittime = static_cast<float>(tot_waittime)/static_cast<float>(event_queue.size());
    float avg_waittime = static_cast<float>(tot_waittime)/numio;

    printf("SUM: %d %d %.2lf %.2lf %d\n", total_time,
    tot_movement, avg_turnaround, avg_waittime, max_waittime);
}

void IOREQS()
{
    cout << "IOREQS INFO" << endl;
    for (int i=0; i<event_queue.size(); i++)
    {
        io_op event = event_queue[i];
        cout << setw(5) << event.index << ":" << setw(6) << event.time << setw(6) << event.start_time << setw(6) << event.end_time << endl;
    }
}

string get_data(string token)
{
    //cout << token << endl;
    string delimiter = "=";
    string real_token = token.substr(token.find(delimiter)+1, token.size()- token.find(delimiter));
    //cout << real_token << endl;
    return real_token;
    //return stoi(real_token);
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

    while ((c = getopt (argc, argv, "s:v:")) != -1)
    {
        switch (c)
        {
            case 's':
                alg = optarg;
                break;
            case 'v':
                verbose = true;
            default:
                abort ();
        }
    }
	ifstream inFile;
	inFile.open(argv[optind]);
	string line;
    Scheduler* sched;

    if (alg == "i")
    {
        sched = new FIFO();
    }
    else if (alg == "j")
    {
        sched = new SSTF();
    }
    else if (alg == "s")
    {
        sched = new SCAN();
    }
    else if (alg == "c")
    {
        // CSCAN sched = CSCAN();
        sched = new CSCAN();
    }
    else if (alg =="f")
    {
        sched = new FSCAN();
        //FSCAN sched = FSCAN();
    }

    int curr_idx = 0;
    bool hash_line = true;
    int hash_line_count= 0;
    while (!inFile.eof())
    {
        bool break_flag = true;
        getline(inFile, line);
        if (!line.empty())
        {
		    string buf; // Have a buffer string
    	    stringstream ss(line); // Insert the string into a stream
    	    vector<string> tokens; // Create vector to hold words
            vector<string> hash_tokens;
    	    while (ss >> buf)
            {
                if (hash_line == true)
                {
                    if (hash_line_count == 2)
                    {
                        tokens.push_back(buf);
                    }
                }
                if (buf[0] == '#')
                {
                    hash_line = true;
                    hash_line_count = (hash_line_count+1);
                    if (hash_line_count == 2)
                    {
                        tokens.push_back(buf);
                    }
                    else
                    {
                        // first line
                        break;
                    }
                    // break_flag = true;
                }
                else
                {
                    // break_flag = false;
            	    tokens.push_back(buf);
                }
            }
            if (hash_line_count == 2)
            {
                numio = stoi(get_data(tokens[0]));
                maxtracks = stoi(get_data(tokens[1]));
                // TODO lambda is wrong
                lambda = get_data(tokens[2]);
                // cout << "numio:" << numio << endl;
                // cout << "maxtracks:" << maxtracks << endl;
                // cout << "lambda:" << lambda << endl;
                hash_line_count = -1;
            }
            else if (hash_line_count == -1)
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

    if (verbose)
    {
        cout << "TRACE" << endl;
    }

    while (!to_add.empty())
    {
        bool did_sth = false;
        // cout << "begin: did sth? " << did_sth << endl;
        // cout << "curr_time: " << curr_time << endl;
        // cout << to_add[0]->time << endl;
        if (to_add[0]->time == curr_time)
        {
            did_sth = true;
            sched->ADD();
        }
        if  (!to_issue.empty())
        {
            if (!io_in_process)
            {
                did_sth = true;
                sched->ISSUE();
            }
            else
            {
                // check if current io can be finished
                // cout << sched->get_index() << endl;
                // cout << to_issue[sched->get_index()]->track_nmbr << endl;
                // if (to_issue[sched->issuing_index]->track_nmbr == curr_track)
                if (to_issue[sched->get_index()]->track_nmbr == curr_track)
                {
                    did_sth = true;
                    sched->FINISH();
                }
            }
        }

        // cout << "did sth? " << did_sth << endl;

        if (!did_sth)
        {
            if (io_in_process)
            // only move cursor if something in process
            {
                if (go_right==true)
                {
                    // curr_track = (curr_track+1)%maxtracks;
                    curr_track++;
                }
                else
                {
                    // curr_track = (curr_track-1)%maxtracks;
                    curr_track--;
                }
                muvement++;
            }
            // cout << "time:" << curr_time << endl;
            curr_time++;
        }
    }
    // cout << "ISSUE_INDEX: " << sched->get_index() << endl;
    while (!to_issue.empty())
    {
        bool did_sth = false;
        if (!io_in_process)
        {
            did_sth = true;
            sched->ISSUE();
        }
        else
        {
            // check if current io can be finished
            // cout << "ISSUE_INDEX: " << sched->get_index() << endl;
            if (to_issue[sched->get_index()]->track_nmbr == curr_track)
            // if (to_issue[sched->issuing_index]->track_nmbr == curr_track)
            {
                did_sth = true;
                sched->FINISH();
            }
        }
        // cout << "did sth? " << did_sth << endl;

        if (!did_sth)
        {
            if (io_in_process)
            // only move cursor if something in process
            {
                if (go_right==true)
                {
                    curr_track++;
                    // curr_track = (curr_track+1)%maxtracks;
                }
                else
                {
                    // curr_track = (curr_track-1)%maxtracks;
                    curr_track--;
                }
                muvement++;
            }
            // cout << "time:" << curr_time << endl;
            // cout << "curr_track:" << curr_track << endl;
            curr_time++;
        }
        // cout << to_issue[0]->index << endl;
    }



    if (verbose)
    {
        IOREQS();
    }
    PRINTSUM(curr_time, muvement);
	inFile.close();
}
