# include <iostream>
# include <fstream>
// # include <ctype.h>
# include <unistd.h>
# include <vector>
# include <sstream>
# include <iomanip>

using namespace std;
vector <int> randvals;
int read_counter = 0;


struct pte
{
    int PRESENT:1;
    int MODIFIED:1;
    int REFERENCED:1;
    int PAGEDOUT:1;
    int idx:8;
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


void UNMAP()
{}

void OUT()
{}

void IN()
{}

void MAP(pte *curr_pte, frame* curr_frame)
{
    cout << read_counter << ": MAP" << setw(6) << setfill(' ') << curr_pte->idx << setw(4) << setfill(' ') << curr_frame->page_index << endl;
    curr_pte->PRESENT = 1;
    curr_pte->REFERENCED = 1;
    curr_frame->pte_ptr = curr_pte;
    fifo_queue.push_back(curr_frame->page_index);
}

void ZERO(frame* curr_frame)
{
    //zero’d (i.e. NOT PRESENT and NOT PAGEDOUT
    pte *curr_pte = curr_frame->pte_ptr;
    // cout << "ZERO"<< setw(9) << setfill(' ') << curr_pte.idx << endl;
    cout << read_counter << ": ZERO"<< setw(9) << setfill(' ') << curr_frame->page_index << endl;
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
    virtual frame* allocate_frame(frame **frame_old)
    {
        return &frametable[0];
    }
};

class FIFO : public Pager{
    public:
        frame* allocate_frame(frame **frame_old)
        {
            // frame_old->pte_ptr.PRESENT = 0;
            frame* frame_to_return = &frametable[fifo_queue.front()];
            fifo_queue.erase (fifo_queue.begin());
            return frame_to_return;
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
        if (pagetable[i].PRESENT == 0)
        {
            cout << "* ";
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
    cout << " || ";
    for (int i=0; i<fifo_queue.size(); i++)
    {
        cout << fifo_queue[i] << ' ';
    }
    cout << endl;
}

frame* allocate_from_free_list()
{
    // frame* frame_to_return = &freelist[0];
    if (freelist.empty())
    {
        return NULL;
    }
    frame* frame_to_return = freelist.front();
    freelist.erase (freelist.begin());
    // cout << "frame from free list:" << frame_to_return->page_index << endl;
    return frame_to_return;
}


frame* get_frame(frame **old_frame)
{
    frame* _frame = allocate_from_free_list();

    if (_frame == NULL)
    {
        // no free frames, got to get old frames
        FIFO fifo = FIFO();
        _frame = fifo.allocate_frame(old_frame);
    }
    else
    {
        *old_frame = NULL;
    }

    return _frame;
}

/*
void result(char c)
{
    cout << c << ": ZERO" << setw(9) << setfill(' ') << frame_used << endl;
        			//cout << c << ": MAP" << setw(6) << setfill(' ')
        			//<< target << setw(4) << setfill(' ') << frame_used << endl;
    cout << line_number << ": IN" << setw(7) << setfill(' ')
    	    		 << p << setw(4) << setfill(' ') << f << endl;
    cout << line_number << ": OUT" << setw(6) << setfill(' ')
    	    			  	 << p << setw(4) << setfill(' ') << f << endl;
}
*/

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
    for (int i=0; i<4; i++)
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
        /*pte initial_pte = pte();
        initial_pte.PRESENT = 0;
        initial_pte.MODIFIED = 0;
        initial_pte.REFERENCED = 0;
        initial_pte.PAGEDOUT = 0;
        initial_pte.idx = i;
        */
        pte initial_pte = {0, 0, 0, 0, i};
        pagetable.push_back(initial_pte);
    }


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
                if (curr_pte->PRESENT == 0) //pte not present
                {
                    frame *old_frame, *new_frame;
                    new_frame = get_frame(&old_frame);
                    if (curr_pte->PAGEDOUT != 0)
                    {
                        IN();
                    }
                    else
                    {
                        ZERO(new_frame);
                    }

                    MAP(curr_pte, new_frame);

                }
                print_pagetable();
                read_counter++;
            }
            else
            {
                break_flag = true;
            }
        }
    }
	inFile.close();
}
