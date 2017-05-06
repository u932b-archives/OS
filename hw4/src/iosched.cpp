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

/*

class RANDOM : public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, int framesize, bool output)
        {
            frame* frame_to_replace = new frame(-1);
            while (frame_to_replace->pte_ptr == NULL )
            {
                int index = myrandom(framesize);
                frame_to_replace = &frametable[index];
            }
            frame_to_replace->pte_ptr->PRESENT = 0;
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;
        }
};

class NRU : public Pager{
    public:
        vector <int> class_0;
        vector <int> class_1;
        vector <int> class_2;
        vector <int> class_3;
        vector <vector<int>*> nru_table;
        frame* allocate_frame(frame* frame_old, stats* stat, int framesize, bool output)
        {
            bool reset = false;
            ++pagedout_counter;
            if (pagedout_counter%10==0)
            {
                reset = true;
            }
            frame* frame_to_replace = new frame(-1);
            vector <int>* curr_class;

            for (int i=0; i<pagetable.size(); i++)
            {
                pte *curr_pte = &pagetable[i];
                if (curr_pte->PRESENT != 0)
                {
                    if (curr_pte->REFERENCED == 0 && curr_pte->MODIFIED == 0)
                    {
                        class_0.push_back(curr_pte->frame_index);
                    }
                    else if (curr_pte->REFERENCED == 0 && curr_pte->MODIFIED != 0)
                    {
                        class_1.push_back(curr_pte->frame_index);
                    }
                    else if (curr_pte->REFERENCED != 0 && curr_pte->MODIFIED == 0)
                    {
                        class_2.push_back(curr_pte->frame_index);
                    }
                    else if (curr_pte->REFERENCED != 0 && curr_pte->MODIFIED != 0)
                    {
                        class_3.push_back(curr_pte->frame_index);
                    }
                    if (reset)
                    {
                        curr_pte->REFERENCED = 0;
                    }
                }
            }

            int class_counter = 0;
            while (class_counter<4 && frame_to_replace->pte_ptr == 0)
            {
                curr_class = nru_table[class_counter];
                //cout << curr_class->size() << endl;
                if (curr_class->size() > 0 )
                {
                    // cout << "frames in curr_class: " << endl;
                    // for (int i=0; i < curr_class->size(); i++)
                    // {
                    //     cout << frametable[(*curr_class)[i]].pte_ptr->idx << ":" << (*curr_class)[i] << " ";
                    // }
                    // cout << endl;

                    int index = myrandom(curr_class->size());
                    int frame_index = (*curr_class)[index];
                    frame_to_replace = &frametable[frame_index];
                    break;
                }
                else
                {
                    class_counter++;
                }
            }
            frame_to_replace->pte_ptr->PRESENT = 0;
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;
        }
};

class SC : public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, bool output)
        {
            frame* frame_to_replace = &frametable[fifo_queue.front()];
            while (frame_to_replace->pte_ptr->REFERENCED!=0)
            {
                int curr_frame_idx = fifo_queue.front();
                frame_to_replace->pte_ptr->REFERENCED=0;
                fifo_queue.erase (fifo_queue.begin());
                fifo_queue.push_back(curr_frame_idx);
                frame_to_replace = &frametable[fifo_queue.front()];
            }
            // Risky fix
            fifo_queue.erase (fifo_queue.begin());

            frame_to_replace->pte_ptr->PRESENT = 0;
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;
        }
};
class CLOCK_PHYS: public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, bool output)
        {
            frame* frame_to_replace = &frametable[fifo_queue.front()];
            while (frame_to_replace->pte_ptr->REFERENCED!=0)
            {
                int curr_frame_idx = fifo_queue.front();
                frame_to_replace->pte_ptr->REFERENCED=0;
                fifo_queue.erase (fifo_queue.begin());
                fifo_queue.push_back(curr_frame_idx);
                frame_to_replace = &frametable[fifo_queue.front()];
            }
            // hand is wrong
            hand = (hand + 1) % frametable.size();
            // Risky fix
            fifo_queue.erase (fifo_queue.begin());

            frame_to_replace->pte_ptr->PRESENT = 0;
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;

        }
};

class CLOCK_VIRTUAL: public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, bool output)
        {
            frame* frame_to_replace;
            while (true)
            {
                // cout << "hand: " << hand << endl;
                pte* curr_pte = &pagetable[hand];
                if (curr_pte->PRESENT == 0)
                {
                    // hand++;
                    hand = (++hand) % pagetable.size();
                }
                else if (curr_pte->REFERENCED != 0 )
                {
                    curr_pte->REFERENCED = 0;
                    hand = (++hand) % pagetable.size();
                }
                else
                {
                    // cout << "found pte:" << curr_pte->idx << endl;
                    // cout << "PRESENT:" << curr_pte->PRESENT << endl;
                    // cout << "REFERENCED:" << curr_pte->REFERENCED << endl;
                    // cout << "pointing to frame:" << curr_pte->frame_index << endl; // this is wrong
                    frame_to_replace = &frametable[curr_pte->frame_index];
                    // cout << "frame_to_replace's pte_ptr:" << frame_to_replace->pte_ptr->idx << endl;
                    break;
                }

            }
            // found frame, point to next
            hand = (hand + 1) % pagetable.size();

            // cout << "frame_to_replace->pte_ptr: " << frame_to_replace->pte_ptr -> idx<< endl;
            frame_to_replace->pte_ptr->PRESENT = 0;
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;
        }
};

class AGE_PHYS: public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, bool output)
        {

            for (int i=0; i<pagetable.size();i++)
            {
                pte *curr_pte = &pagetable[i];
                //pte *curr_pte = frametable[i].pte_ptr;
                if (curr_pte->PRESENT != 0)
                //if (curr_pte->REFERENCED != 0 && curr_pte->PRESENT != 0)
                {
                    curr_pte->age_bit_vector = curr_pte->age_bit_vector >> 1;
                    if (curr_pte->REFERENCED != 0)
                    {
                        curr_pte->age_bit_vector.flip(31);
                        curr_pte->REFERENCED=0;
                    }
                }
            }
            frame* frame_to_replace = &frametable[0];
            bitset<32> tmp(std::string("11111111111111111111111111111111"));

            pte initial_pte = {0, 0, 0, 0, -1, 0, tmp};
            pte* smallest_pte = &initial_pte;


            for (int i=0; i<frametable.size(); i++)
            {
                for (int j = 32-1; j >= 0; j--) {
        	 		if (smallest_pte->age_bit_vector[j] && !frametable[i].pte_ptr->age_bit_vector[j])
			 		{
                         smallest_pte = frametable[i].pte_ptr;
                         frame_to_replace = &frametable[i];
                         break;
                     }

        	 		else if (!smallest_pte->age_bit_vector[j] && frametable[i].pte_ptr->age_bit_vector[j])
			 		{
                         break;
			 		}

    		 	}
            }
            // cout << "min_pte = " << smallest_pte->idx << " age=" << smallest_pte->age_bit_vector << endl;
            frame_to_replace->pte_ptr->PRESENT = 0;
            frame_to_replace->pte_ptr->age_bit_vector = 0;
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;
        }
};

class AGE_VIRTUAL: public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, bool output)
        {
            for (int i=0; i<pagetable.size();i++)
            {
                pte *curr_pte = &pagetable[i];
                //pte *curr_pte = frametable[i].pte_ptr;
                if (curr_pte->PRESENT != 0)
                {
                    curr_pte->age_bit_vector = curr_pte->age_bit_vector >> 1;
                    if (curr_pte->REFERENCED != 0)
                    {
                        curr_pte->age_bit_vector.flip(31);
                        curr_pte->REFERENCED=0;
                    }
                    // cout << curr_pte->idx << ":" << curr_pte->age_bit_vector << endl;
                }
            }
            frame* frame_to_replace = &frametable[0];
            bitset<32> tmp(std::string("11111111111111111111111111111111"));

            pte initial_pte = {0, 0, 0, 0, -1, 0, tmp};
            //pte* smallest_pte = frametable[0].pte_ptr;
            pte* smallest_pte = &initial_pte;

            for (int i=0; i<pagetable.size();i++)
            {
                pte *curr_pte = &pagetable[i];
                if (curr_pte->PRESENT != 0)
                {
                    for (int j = 32-1; j >= 0; j--)
                    {
        	 		    if (smallest_pte->age_bit_vector[j] && !pagetable[i].age_bit_vector[j])
			 		    {
                            // cout << "replacing smallest:" << smallest_pte->idx << " with " << pagetable[i].idx << endl;
                             smallest_pte = &pagetable[i];
                             frame_to_replace = &frametable[pagetable[i].frame_index];
                             // continue;
                             break;
                         }

        	 		    else if (!smallest_pte->age_bit_vector[j] && pagetable[i].age_bit_vector[j])
			 		    {
                             // continue;
                             break;
                        }
                    }
                }
            }
            // cout << "min_pte = " << smallest_pte->idx << " age=" << smallest_pte->age_bit_vector << endl;
            frame_to_replace->pte_ptr->PRESENT = 0;
            frame_to_replace->pte_ptr->age_bit_vector = 0;
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;
        }
};

void print_pagetable()
{
    for (int i=0; i<pagetable.size(); i++)
    {
        // if (pagetable[i].PRESENT == 0 && pagetable[i].MODIFIED == 0)
        if (pagetable[i].PRESENT == 0 && pagetable[i].PAGEDOUT == 0)
        {
            cout << "* ";
        }
        else if (pagetable[i].PRESENT == 0 && pagetable[i].PAGEDOUT != 0)
        // else if (pagetable[i].PRESENT == 0 && pagetable[i].MODIFIED != 0)
        {
            cout << "# ";
        }
        else
        {
            char REF, MOD, PGOUT;

            if (pagetable[i].REFERENCED != 0) // is -1
            {
                REF = 'R';
            }
            else
            {
                REF = '-';
            }
            if (pagetable[i].MODIFIED != 0) // is -1
            {
                MOD = 'M';
            }
            else
            {
                MOD = '-';
            }
            if (pagetable[i].PAGEDOUT != 0) // is -1
            {
                PGOUT = 'S';
            }
            else
            {
                PGOUT = '-';
            }
            cout << i << ':' << REF << MOD << PGOUT << ' ';
        }
    }
    cout << endl;
}

void print_frametable(string alg, bool debug)
{
    for (int i=0; i < frametable.size(); i++)
    {
        if (frametable[i].pte_ptr == 0)
        {
            cout << "* ";
        }
        else
        {
            cout << frametable[i].pte_ptr->idx << ' ';
        }
    }
    if ((alg=="f"|| alg == "s") && debug)
    {
        cout << " || ";
        for (int i=0; i<fifo_queue.size(); i++)
        {
            cout << fifo_queue[i] << ' ';
        }
    }

    if ((alg=="c" || alg == "X") && debug)
    {
        cout << " || hand = " << hand;
    }
    cout << endl;
}

void print_sum(stats stat)
{
    printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
    inst_count, stat.unmaps, stat.maps, stat.ins, stat.outs, stat.zeros, stat.totalcost);
}


frame* allocate_from_free_list()
{
    if (freelist.empty())
    {
        return 0;
    }
    frame* frame_to_return = freelist.front();
    freelist.erase (freelist.begin());
    return frame_to_return;
}


frame* get_frame(frame* old_frame, stats* stat, string alg, int framesize, bool output)
{
    frame* _frame = allocate_from_free_list();

    if (_frame == 0)
    {
        // no free frames, got to get old frames
        if (alg == "i")
        {
            FIFO fifo = FIFO();
            _frame = fifo.allocate_frame(old_frame, stat, output);
        }
        else if (alg == "r")
        {
            RANDOM random_alg = RANDOM();
            _frame = random_alg.allocate_frame(old_frame, stat, framesize, output);
        }
        else if (alg == "s")
        {
            SC sc_alg = SC();
            _frame = sc_alg.allocate_frame(old_frame, stat, output);
        }
        else if (alg == "N")
        {
            NRU nru_alg = NRU();
            nru_alg.nru_table = {&nru_alg.class_0, &nru_alg.class_1, &nru_alg.class_2, &nru_alg.class_3};

            _frame = nru_alg.allocate_frame(old_frame, stat, framesize, output);
        }
        else if (alg == "c")
        {
            CLOCK_PHYS clk_phys_alg = CLOCK_PHYS();
            _frame = clk_phys_alg.allocate_frame(old_frame, stat, output);
        }
        else if (alg == "X")
        {
            CLOCK_VIRTUAL clk_vir_alg = CLOCK_VIRTUAL();
            _frame = clk_vir_alg.allocate_frame(old_frame, stat, output);
        }
        else if (alg == "a")
        {
            AGE_PHYS age_phys_alg = AGE_PHYS();
            _frame = age_phys_alg.allocate_frame(old_frame, stat, output);
        }
        else if (alg == "Y")
        {
            AGE_VIRTUAL age_virtual_alg = AGE_VIRTUAL();
            _frame = age_virtual_alg.allocate_frame(old_frame, stat, output);
        }
    }
    else
    {
        old_frame = 0;
    }

    return _frame;
}
*/

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

    vector<io_op> to_issue;
    int curr_time = 1;
    bool io_in_process = false;
    int curr_track = 0;
    while (!event_queue.empty())
    {
        if (event_queue[0].time == curr_time)
        {
            //do something
            cout << curr_time << " " << event_queue[0].index << " add " <<
                event_queue[0].track_nmbr << endl;

            event_queue[0].start_time = curr_time;

            if (!io_in_process)
            {
                if (to_issue.empty())
                {
                    cout << curr_time << " " << event_queue[0].index << " issue " <<
                    event_queue[0].track_nmbr << " " << curr_track << endl;
                    io_in_process = true;
                    to_issue.push_back(event_queue[0]);
                }
                else
                {
                    to_issue.push_back(event_queue[0]);
                }
            }
            event_queue.erase(event_queue.begin());
        }
        // else if (!to_issue.empty())
        // {
            if (to_issue[0].track_nmbr == curr_time - to_issue[0].start_time)
            {
                cout << curr_time << " " << to_issue[0].index << " finish " <<
                        to_issue[0].track_nmbr << endl;
                to_issue.erase(to_issue.begin());
                io_in_process = false;
            }
        // }
        if (!io_in_process)
        {

        }
        else
        {
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
