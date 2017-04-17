# include <iostream>
# include <fstream>
// # include <ctype.h>
# include <unistd.h>
# include <vector>
# include <sstream>
# include <iomanip>

using namespace std;
vector <int> randvals;
int inst_count = 0;


struct pte
{
    int PRESENT:1;
    int MODIFIED:1;
    int REFERENCED:1;
    int PAGEDOUT:1;
    int idx:8;
};

struct stats
{
    int unmaps;
    int maps;
    int ins;
    int outs;
    int zeros;
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
    pte_ptr = nullptr;
}


vector <pte> pagetable;
vector <frame> frametable;
vector <frame *> freelist;
vector <int> fifo_queue;


void UNMAP(frame* frame_to_replace, stats* stat)
{
    cout << inst_count << ": UNMAP" << setw(4) << setfill(' ') << frame_to_replace->pte_ptr->idx << setw(4) << setfill(' ') << frame_to_replace->page_index << endl;
    stat->unmaps++;
}

void OUT(frame* frame_to_replace, stats* stat)
{
    cout << inst_count << ": OUT" << setw(6) << setfill(' ') << frame_to_replace->pte_ptr->idx << setw(4) << setfill(' ') << frame_to_replace->page_index << endl;
    frame_to_replace->pte_ptr->PAGEDOUT = 1;
    frame_to_replace->pte_ptr->MODIFIED = 0;
    stat->outs++;
}

void IN(pte *curr_pte, frame* curr_frame, stats* stat)
{
    cout << inst_count << ": IN" << setw(7) << setfill(' ') << curr_pte->idx << setw(4) << setfill(' ') << curr_frame->page_index << endl;
    // curr_pte->MODIFIED = 0;
    stat->ins++;
}

void MAP(pte *curr_pte, frame* curr_frame, stats* stat)
{
    cout << inst_count << ": MAP" << setw(6) << setfill(' ') << curr_pte->idx << setw(4) << setfill(' ') << curr_frame->page_index << endl;
    stat->maps++;
    curr_pte->PRESENT = 1;
    curr_pte->REFERENCED = 1;
    curr_frame->pte_ptr = curr_pte;
    fifo_queue.push_back(curr_frame->page_index);
}

void ZERO(frame* curr_frame, stats* stat)
{
    //zero’d (i.e. NOT PRESENT and NOT PAGEDOUT
    pte *curr_pte = curr_frame->pte_ptr;
    // cout << "ZERO"<< setw(9) << setfill(' ') << curr_pte.idx << endl;
    cout << inst_count << ": ZERO"<< setw(9) << setfill(' ') << curr_frame->page_index << endl;
    stat->zeros++;
    if (curr_pte != NULL || curr_pte != nullptr)
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
}


class Pager {
    virtual frame* allocate_frame(frame *frame_old)
    {
        return &frametable[0];
    }
};

class FIFO : public Pager{
    public:
        frame* allocate_frame(frame* frame_old, stats* stat)
        {
            frame* frame_to_replace = &frametable[fifo_queue.front()];
            frame_to_replace->pte_ptr->PRESENT = 0;
            fifo_queue.erase (fifo_queue.begin());
            UNMAP(frame_to_replace, stat);
            if (frame_to_replace->pte_ptr->MODIFIED != 0)
            {
                OUT(frame_to_replace, stat);
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

void print_pagetable(bool end_result=false, char alg=' ', stats stat={0,0,0,0,0})
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
    if (alg=='f' && !end_result)
    {
        cout << " || ";
        for (int i=0; i<fifo_queue.size(); i++)
        {
            cout << fifo_queue[i] << ' ';
        }
        cout << endl;
    }
    if (end_result)
    {
        cout << endl;
        long long totalcost = 400*(stat.unmaps+stat.maps) + 3000*(stat.ins+stat.outs) + 150*stat.zeros+inst_count;
        printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
        inst_count, stat.unmaps, stat.maps, stat.ins, stat.outs, stat.zeros, totalcost);
    }
}


frame* allocate_from_free_list()
{
    // frame* frame_to_return = &freelist[0];
    if (freelist.empty())
    {
        return nullptr;
    }
    frame* frame_to_return = freelist.front();
    freelist.erase (freelist.begin());
    // cout << "frame from free list:" << frame_to_return->page_index << endl;
    return frame_to_return;
}


frame* get_frame(frame* old_frame, stats* stat)
{
    frame* _frame = allocate_from_free_list();

    if (_frame == nullptr)
    {
        // no free frames, got to get old frames
        FIFO fifo = FIFO();
        _frame = fifo.allocate_frame(old_frame, stat);
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

    while ((c = getopt (argc, argv, "aof")) != -1)
    {
        switch (c)
        {
            case 'o':
                // cout << "verbose" << endl;
                break;
            case 'O':
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
    for (int i=0; i<16; i++)
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
                cout << "==> inst: " << tokens[0] << " " << tokens[1] << endl;
                pte *curr_pte = &pagetable[stoi(tokens[1])];
                if (stoi(tokens[0]) == 1) //write operation
                {
                    curr_pte->MODIFIED=1;
                }
                if (curr_pte->PRESENT == 0) //pte not present
                {
                    frame *old_frame, *new_frame;
                    new_frame = get_frame(old_frame, &stat);
                    if (curr_pte->PAGEDOUT != 0)
                    {
                        IN(curr_pte, new_frame, &stat);
                    }
                    else
                    {
                        ZERO(new_frame, &stat);
                    }

                    MAP(curr_pte, new_frame, &stat);

                }
                print_pagetable(false, 'f', stat);
                inst_count++;
            }
            else
            {
                break_flag = true;
            }
        }
    }
    print_pagetable(true, 'f', stat);
	inFile.close();
}
