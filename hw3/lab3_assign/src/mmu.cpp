# include <iostream>
# include <fstream>
// # include <ctype.h>
# include <unistd.h>
# include <vector>
# include <sstream>
# include <iomanip>
#include <bitset>


using namespace std;
vector <int> randvals;
int inst_count = 0;

int hand=0;
int pagedout_counter=0;

struct pte
{
    int PRESENT:1;
    int MODIFIED:1;
    int REFERENCED:1;
    int PAGEDOUT:1;
    int idx:8;
    int frame_index:8;
    // vector <bool> age_bit_vector;
    bitset<32> age_bit_vector;
};

class stats
{
    public:
        int unmaps;
        int maps;
        int ins;
        int outs;
        int zeros;
        long long totalcost;

};

class frame
{
    public:
        int page_index;
        pte *pte_ptr;
        frame(int idx);
};

frame::frame(int idx)
{
    page_index = idx;
    pte_ptr = 0;
    //pte_ptr = nullptr;
}


vector <pte> pagetable;
vector <frame> frametable;
vector <frame *> freelist;
vector <int> fifo_queue;

void UNMAP(frame* frame_to_replace, stats* stat, bool output)
{
    if (output)
    {
        cout << inst_count << ": UNMAP" << setw(4) << setfill(' ') << frame_to_replace->pte_ptr->idx << setw(4) << setfill(' ') << frame_to_replace->page_index << endl;
    }
    stat->unmaps++;
    stat->totalcost = stat->totalcost + 400;
}

void OUT(frame* frame_to_replace, stats* stat, bool output)
{
    if (output)
    {
    cout << inst_count << ": OUT" << setw(6) << setfill(' ') << frame_to_replace->pte_ptr->idx << setw(4) << setfill(' ') << frame_to_replace->page_index << endl;
    }
    frame_to_replace->pte_ptr->PAGEDOUT = 1;
    frame_to_replace->pte_ptr->MODIFIED = 0;
    stat->outs++;
    stat->totalcost = stat->totalcost + 3000;
}

void IN(pte *curr_pte, frame* curr_frame, stats* stat, bool output)
{
    if (output)
    {
        cout << inst_count << ": IN" << setw(7) << setfill(' ') << curr_pte->idx << setw(4) << setfill(' ') << curr_frame->page_index << endl;
    }
    stat->ins++;
    stat->totalcost = stat->totalcost + 3000;
}

void MAP(pte *curr_pte, frame* curr_frame, stats* stat, bool output)
{
    if (output)
    {
        cout << inst_count << ": MAP" << setw(6) << setfill(' ') << curr_pte->idx << setw(4) << setfill(' ') << curr_frame->page_index << endl;
    }
    stat->maps++;
    curr_pte->PRESENT = 1;
    curr_pte->REFERENCED = 1;
    curr_frame->pte_ptr = curr_pte;
    curr_pte->frame_index = curr_frame->page_index;
    fifo_queue.push_back(curr_frame->page_index);
    stat->totalcost = stat->totalcost + 400;
}

void ZERO(frame* curr_frame, stats* stat, bool output)
{
    //zero’d (i.e. NOT PRESENT and NOT PAGEDOUT
    pte *curr_pte = curr_frame->pte_ptr;
    if (output)
    {
        cout << inst_count << ": ZERO"<< setw(9) << setfill(' ') << curr_frame->page_index << endl;
    }
    stat->zeros++;
    if (curr_pte != NULL)
    // if (curr_pte != NULL || curr_pte != nullptr)
    {
        // curr_pte->PRESENT = 0;
        // curr_pte->PAGEDOUT = 0;
        // cout << "yo man" << endl;
        curr_frame->pte_ptr = nullptr;
    }
    else
    {
        // cout << "Already nullptr, peace." << endl;
    }
    stat->totalcost = stat->totalcost + 150;
}


class Pager {
    virtual frame* allocate_frame(frame *frame_old)
    {
        return &frametable[0];
    }
};

class FIFO : public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, bool output)
        {
            frame* frame_to_replace = &frametable[fifo_queue.front()];
            frame_to_replace->pte_ptr->PRESENT = 0;
            fifo_queue.erase (fifo_queue.begin());
            UNMAP(frame_to_replace, stat, output);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat, output);
            }
            return frame_to_replace;
        }
};

int ofs = 0;
int myrandom(int framesize)
{
    ofs = ofs % randvals.size();
    int to_return = (randvals[ofs] % framesize);
    // cout << to_return << endl;
    ofs++;
    return to_return;
}

class RANDOM : public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat, int framesize, bool output)
        {
            frame* frame_to_replace = new frame(-1);
            while (frame_to_replace->pte_ptr == NULL )
            // while (frame_to_replace->pte_ptr == nullptr||frame_to_replace->pte_ptr == NULL )
            {
                int index = myrandom(framesize);
                // cout << index << endl;
                frame_to_replace = &frametable[index];
                // cout << frame_to_replace->pte_ptr->idx << endl;
            }
            frame_to_replace->pte_ptr->PRESENT = 0;
            // fifo_queue.erase (fifo_queue.begin());
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
        vector <vector<int>*> nru_table = {&class_0, &class_1, &class_2, &class_3};
        frame* allocate_frame(frame* frame_old, stats* stat, int framesize, bool output)
        {
            bool reset = false;
            ++pagedout_counter;
            // cout << "counter: " << pagedout_counter << endl;
            if (pagedout_counter%10==0)
            {
                reset = true;
            }
            frame* frame_to_replace = new frame(-1);
            // go through the whole frametable
            vector <int>* curr_class;

            //for (int i=0; i<frametable.size(); i++)
            for (int i=0; i<pagetable.size(); i++)
            {
                pte *curr_pte = &pagetable[i];
                //pte *curr_pte = frametable[i].pte_ptr;
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
            // while (class_counter<4 && frame_to_replace->pte_ptr == nullptr)
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
            // int curr_frame_idx = fifo_queue.front();
            frame* frame_to_replace = &frametable[fifo_queue.front()];
            // fifo_queue.push_back(curr_frame_idx);
            // fifo_queue.erase (fifo_queue.begin());
            while (frame_to_replace->pte_ptr->REFERENCED!=0)
            {
                int curr_frame_idx = fifo_queue.front();
                frame_to_replace->pte_ptr->REFERENCED=0;
                fifo_queue.erase (fifo_queue.begin());
                fifo_queue.push_back(curr_frame_idx);
                // fifo_queue.push_back(frame_to_replace->pte_ptr->idx);
                frame_to_replace = &frametable[fifo_queue.front()];
                // fifo_queue.erase (fifo_queue.begin());
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
                    // cout << curr_pte->idx << ":" << curr_pte->age_bit_vector << endl;
                }
            }
            frame* frame_to_replace = &frametable[0];
            bitset<32> tmp(std::string("11111111111111111111111111111111"));

            pte initial_pte = {0, 0, 0, 0, -1, 0, tmp};
            //pte* smallest_pte = frametable[0].pte_ptr;
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
            /*
            for (int i=0; i<pagetable.size();i++)
            {
                pte *curr_pte = &pagetable[i];
                if (curr_pte->PRESENT != 0)
                {
                    // cout << i << endl;
                    bool break_flag = false;
                    for (int j = 32-1; j >= 0; j--)
                    {
                        if (!break_flag){
        	 		    if (smallest_pte->age_bit_vector[j] && !pagetable[i].age_bit_vector[j])
			 		    {
                            // cout << "replacing smallest:" << smallest_pte->idx << " with " << pagetable[i].idx << endl;
                             smallest_pte = &pagetable[i];
                             frame_to_replace = &frametable[pagetable[i].frame_index];
                             // continue;
                             // break;
                             break_flag = true;
                         }

        	 		    else if (!smallest_pte->age_bit_vector[j] && pagetable[i].age_bit_vector[j])
			 		    {
                             // continue;
                             //break;
                             break_flag = true;
                        }
                        }
                    }
                }
            }
            */
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
                //if (curr_pte->REFERENCED != 0 && curr_pte->PRESENT != 0)
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
            /*
            for (int i=0; i<frametable.size(); i++)
                for (int j = 32-1; j >= 0; j--) {
        	 		if (smallest_pte->age_bit_vector[j] && !frametable[i].pte_ptr->age_bit_vector[j])
			 		{
                         smallest_pte = frametable[i].pte_ptr;
                         frame_to_replace = &frametable[i];
                         break;
                     }

        	 		else if (!smallest_pte->age_bit_vector[i] && frametable[i].pte_ptr->age_bit_vector[j])
			 		{
                         break;
			 		}

    		 	}
            }*/

            for (int i=0; i<pagetable.size();i++)
            {
                pte *curr_pte = &pagetable[i];
                if (curr_pte->PRESENT != 0)
                {
                    // cout << i << endl;
                    bool break_flag = false;
                    for (int j = 32-1; j >= 0; j--)
                    {
                        if (!break_flag){
        	 		    if (smallest_pte->age_bit_vector[j] && !pagetable[i].age_bit_vector[j])
			 		    {
                            // cout << "replacing smallest:" << smallest_pte->idx << " with " << pagetable[i].idx << endl;
                             smallest_pte = &pagetable[i];
                             frame_to_replace = &frametable[pagetable[i].frame_index];
                             // continue;
                             // break;
                             break_flag = true;
                         }

        	 		    else if (!smallest_pte->age_bit_vector[j] && pagetable[i].age_bit_vector[j])
			 		    {
                             // continue;
                             //break;
                             break_flag = true;
                        }
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
void test_free_list()
{
    int i = 0;
    cout << "freelist:" << endl;
    while (i< freelist.size())
    {
        cout << freelist[i]->page_index << " ";
        i++;
    }
    cout << endl;
    cout << "frametable" << endl;
    for (int i=0; i < frametable.size(); i++)
    {
        cout << frametable[i].page_index << " ";
    }
    cout << endl;
}

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
    // int64_t totalcost2 = 400*(stat.unmaps+stat.maps) + 3000*(stat.ins+stat.outs) + 150*stat.zeros + inst_count;
    // long long totalcost = 400*(stat.unmaps+stat.maps) + 3000*(stat.ins+stat.outs) + 150*stat.zeros + inst_count;
    printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
    inst_count, stat.unmaps, stat.maps, stat.ins, stat.outs, stat.zeros, stat.totalcost);
}


frame* allocate_from_free_list()
{
    if (freelist.empty())
    {
        return 0;
        // return nullptr;
    }
    frame* frame_to_return = freelist.front();
    freelist.erase (freelist.begin());
    return frame_to_return;
}


frame* get_frame(frame* old_frame, stats* stat, string alg, int framesize, bool output)
{
    frame* _frame = allocate_from_free_list();

    if (_frame == 0)
    // if (_frame == nullptr)
    {
        // no free frames, got to get old frames
        if (alg == "f")
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
        old_frame = nullptr;
    }

    return _frame;
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

    int framesize=4;
    bool ordinary_flag=false;
    bool print_flag=false;
    bool debug_print_flag=false;
    bool sum_flag=false;
    bool frametable_flag=false;
    bool debug_frametable_flag=false;
    string alg;
    string tmp;

    while ((c = getopt (argc, argv, "a:o:f:")) != -1)
    {
        switch (c)
        {
            case 'a':
                alg = optarg;
                break;
            case 'o':
                // {
                // string str(optarg);
                tmp = optarg;
                if (tmp.find("O", 0, 1)!= string::npos)
                {
                    ordinary_flag=true;
                }
                if (tmp.find("p", 0, 1)!= string::npos)
                {
                    debug_print_flag=true;
                }
                if (tmp.find("P", 0, 1)!=string::npos)
                {
                    print_flag=true;
                }
                if (tmp.find("S", 0, 1)!=string::npos)
                {
                    sum_flag=true;
                }
                if (tmp.find("f", 0, 1)!=string::npos)
                {
                    debug_frametable_flag=true;
                }
                if (tmp.find("F", 0, 1)!=string::npos)
                {
                    frametable_flag=true;
                }

                break;
                //}
            case 'f':
                framesize = atoi(optarg);
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

    // initiate frametable, freelist, pagetable
    for (int i=0; i<framesize; i++)
    {
        frame a_frame = frame(i);
        // a_frame.page_index = i;
        // a_frame.pte_ptr = nullptr;
        frametable.push_back(a_frame);
    }

    for (int i=0; i<frametable.size(); i++)
    {
        freelist.push_back(&frametable[i]);
    }

    for (int i=0; i<64; i++)
    {
        pte initial_pte = {0, 0, 0, 0, i};
        pagetable.push_back(initial_pte);
    }

    // initiate NRU class
    /*
    if (alg=="N")
    {
        for (int i=0; i<64; i++)
        {
            class_0.push_back(pagetable[i]);
        }
    }*/

    stats stat = {0,0,0,0,0};

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
            if (break_flag == false){
                // test_free_list();
                stat.totalcost++;
                if (ordinary_flag)
                {
                    cout << "==> inst: " << tokens[0] << " " << tokens[1] << endl;
                }
                pte *curr_pte = &pagetable[stoi(tokens[1])];
                curr_pte->REFERENCED=1;
                if (stoi(tokens[0]) == 1) //write operation
                {
                    curr_pte->MODIFIED=1;
                }
                if (curr_pte->PRESENT == 0) //pte not present
                {
                    frame *old_frame, *new_frame;
                    new_frame = get_frame(old_frame, &stat, alg, framesize, ordinary_flag);
                    if (curr_pte->PAGEDOUT != 0)
                    {
                        IN(curr_pte, new_frame, &stat, ordinary_flag);
                    }
                    else
                    {
                        ZERO(new_frame, &stat, ordinary_flag);
                    }

                    MAP(curr_pte, new_frame, &stat, ordinary_flag);

                }
                if (debug_print_flag)
                {
                    print_pagetable();
                }
                // TODO
                //if (frametable_flag && debug_print_flag)
                if (debug_frametable_flag)
                {
                    print_frametable(alg, debug_frametable_flag);
                }
                inst_count++;
            }
            else
            {
                break_flag = true;
            }
        }
    }
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
	inFile.close();
}
