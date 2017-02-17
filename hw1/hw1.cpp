# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <map>
# include <string>
# include <iostream>
# include <vector>

using namespace std;

#define pass (void)0
int max_arr_size = 17;
int machine_size = 512;

/*
class ObjFactory
{
  public:
    static Box *newBox(const std::string &description)
    {
      if (description == "pretty big box")
        return new PrettyBigBox;
      if (description == "small box")
        return new SmallBox;
      return 0;
    }
};*/

class Symbol {
  public:
    int value;
    void set_values (int);
};

void Symbol::set_values (int y) {
  value = y;
}

class Def_Symbol : public Symbol{
    public:
    int defined_module;
    bool has_duplicate;
    bool is_used;
    void set_values (int, bool, bool, int);
};

class Use_Symbol : public Symbol{
    public:
    string symbol_name;
    int defined_module;
    bool is_defined;
    void set_values (string&, bool, int, bool);
    bool is_used;
};

void Use_Symbol::set_values(string& name, bool defined, int module, bool used)
{
    symbol_name = name;
    // defined in def list or not
    is_defined = defined;
    defined_module = module;
    is_used = used;
}

void Def_Symbol::set_values (int val, bool dup, bool used, int module) {
  has_duplicate = dup;
  value = val;
  is_used = used;
  defined_module = module;
}

int main(int argc, char *argv[])
{
	if ( argc == 2 ) {
        pass;
    }
    else if ( argc > 2 ) {
        fprintf (stderr, "Too many arguments supplied.\n");
        exit (1);
    }
    else {
        fprintf (stderr, "One argument expected.\n");
        exit (1);
    }

    char *input_file = argv[1];
    FILE *ifp, *ofp;
    // char *mode = "r";
    char output_file[] = "out.list";
    char c;
    int cur_section = 0;

    map<string, Def_Symbol> symbol_table;

    ifp = fopen(input_file, "r");
    if (ifp)
    {
        // flags
        int new_section = 1;
        int fookin_delim = 0;
        int expect_symbol = 0;
        int module_start = 1;

        // section cur_section = def_list;

        // counters
        int section_counter = 0;
        int symbol_count = -1;
        int base_address = 0;
        int cur_module = 1;

        string tmp_arr;

        string map_key;


        while ((c = getc(ifp)) != EOF)
        {
            if (c != '\t' && c != '\n' && c != ' ')
            {
                fookin_delim = 0;
                expect_symbol = 0;

                if (new_section == 1)
                {
                    // This would print as many times as the number of char
                    // in the section_counter
                    // printf ("We are at section: %d\n", (int)cur_section);
                    tmp_arr+=c;
                }
                else
                {
                    switch (cur_section)
                    {
                        case 0:
                            // printf ("current c: %c\n", c);
                            // printf ("section_counter is: %d\n"
                            // , section_counter);
                            module_start = 1;
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                // This many symbols before next section
                                symbol_count = section_counter*2;
                            }
                            // printf ("symbol_count is: %d\n", symbol_count);
                            if (symbol_count % 2 == 1 && !isdigit(c))
                            {
                                printf ("Error: Should be a digit instead:"
                                        "%c\n", c);
                            }
                            tmp_arr+=c;
                            break;

                        case 1:
                            // printf ("current c: %c\n", c);
                            // printf ("section_counter is: %d\n", section_counter);
                            // printf ("symbol_count is: %d\n", symbol_count);
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter;
                            }
                            tmp_arr+=c;
                            break;

                        case 2:
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter * 2;
                                base_address += section_counter;
                            }
                            // printf ("symbol_count is: %d\n", symbol_count);
                            tmp_arr+=c;
                            break;

                        default:
                            pass;
                    }
                }
            }
            else
            // We hit an delimiter, time to flush tmp_arr
            {
                if (fookin_delim == 1 )
                {
                    continue;
                }
                if (new_section == 1)
                {
                    // Case when input file start with delimiter
                    if (tmp_arr[0] == '\0')
                    {
                        continue;
                    }

                    section_counter = std::stoi (tmp_arr);

                    if (section_counter == 0)
                    {
                        // printf ("Hitting zero\n");
                        cur_section++;
                        cur_section = cur_section % 3;
                        section_counter = 0;
                        // printf ("We are at section: %d\n", (int)cur_section);
                    }
                    else
                    {
                        // printf ("section_counter is: %d\n", section_counter);
                        new_section = 0;
                    }
                    tmp_arr.clear();
                    fookin_delim = 1;
                    continue;
                }
                if (fookin_delim == 0)
                {
                    // Gotta write to symbol_table
                    if (cur_section == 0)
                    {
                        if (symbol_count % 2 == 0)
                        {
                            map_key = tmp_arr;
                        }
                        else if (symbol_count % 2 == 1)
                        {
                            if (symbol_table.find(map_key) == symbol_table.end())
                            {
                                Def_Symbol a_def_symbol;
                                int symbol_addr = base_address + stoi(tmp_arr);
                                a_def_symbol.set_values(symbol_addr, false,
                                        false, cur_module);
                                symbol_table[map_key] = a_def_symbol;
                            }
                            else
                            {
                                symbol_table[map_key].has_duplicate = true;
                            }
                        }
                    }

                    // flush
                    tmp_arr.clear();

                    if (symbol_count!= -1)
                    {
                        symbol_count--;
                        if (symbol_count == 0)
                        {
                            // printf ("moving on to the next section.\n");
                            symbol_count = -1;
                            cur_section++;
                            cur_section = cur_section % 3;
                            new_section = 1;
                            module_start = 0;
                            if (cur_section == 0)
                            {
                                cur_module++;
                            }
                        }
                    }
                    else
                    {
                        expect_symbol = 1;
                    }
                }
                fookin_delim = 1;
                // continue;
            }
        }
        if (module_start != 0 || cur_section != 0)
        {
            printf ("Error: File ended before finishing a module\n");
        }
        if (symbol_count != -1 || expect_symbol == 1)
        {
            // TODO
            // Find where the fuck the wrong is.
            printf ("Input finished before sufficient symbols!\n");
        }
        fclose(ifp);

	    for( std::map<string, Def_Symbol>::iterator iter = symbol_table.begin();
         	iter != symbol_table.end();
         	++iter )
            if (iter->second.value > base_address-1)
            {
                cout << "Warning: Module " << iter->second.defined_module <<
                    ": " << iter->first <<" too big " <<
                    iter->second.value << " (max=" << base_address-1 <<
                    ") assume zero relative\n";
                symbol_table[iter->first].value = 0;
            }

        printf ("Symbol Table\n");
	    for( std::map<string, Def_Symbol>::iterator iter = symbol_table.begin();
         	iter != symbol_table.end();
         	++iter )
	    {
            if (iter->second.has_duplicate == false)
            {
                std::cout << iter->first << "=" << iter->second.value << "\n";
            }
            else
            {
                std::cout << iter->first << "=" << iter->second.value <<
                " Error: This variable is multiple times defined; first value used"
                << "\n";
            }
	    }
    }
    else
    {
        fprintf (stderr, "Can't open file %s\n", input_file);
        exit (1);
    }

    // pass2
    ifp = fopen(input_file, "r");
    if (ifp)
    {
        // flags
        int new_section = 1;
        int fookin_delim = 0;
        int expect_symbol = 0;
        int module_start = 1;

        cur_section = 0;

        // counters
        int section_counter = 0;
        int symbol_count = -1;
        int base_address = 0;
        int prev_section_counter = 0;
        int addr_counter = 0;
        int cur_module = 1;

        // tmp array
        string tmp_arr;

        // use list
        vector<Use_Symbol> le_use_list;
        // vector<Use_Symbol> unused_list;

        // tmp storage
        char tmp_prog_txt;

        printf ("\nMemory Map\n");
        // printf ("use count: %d\n", use_count);
        while ((c = getc(ifp)) != EOF)
        {
            //printf ("%c", c);
            if (c != '\t' && c != '\n' && c != ' ')
            {
                // printf ("c is %c\n", c);
                fookin_delim = 0;
                expect_symbol = 0;

                if (new_section == 1)
                {
                    // This would print as many times as the number of char
                    // in the section_counter
                    // printf ("We are at section: %d\n", (int)cur_section);
                    tmp_arr+=c;
                }
                else
                {
                    switch (cur_section)
                    {
                        case 0:
                            // printf ("current c: %c\n", c);
                            // printf ("section_counter is: %d\n"
                            // , section_counter);
                            module_start = 1;
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                // This many symbols before next section
                                symbol_count = section_counter*2;
                            }
                            // printf ("symbol_count is: %d\n", symbol_count);
                            if (symbol_count % 2 == 1 && !isdigit(c))
                            {
                                printf ("Error: Should be a digit instead:"
                                        "%c\n", c);
                            }
                            tmp_arr+=c;
                            break;
                        case 1:
                            // printf ("current c: %c\n", c);
                            // printf ("section_counter is: %d\n", section_counter);
                            // printf ("symbol_count is: %d\n", symbol_count);
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter;
                            }
                            tmp_arr+=c;
                            break;
                        case 2:
                            // printf ("section_counter is: %d\n", section_counter);
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter * 2;
                                base_address += prev_section_counter;
                                prev_section_counter = section_counter;
                            }
                            // printf ("symbol_count is: %d\n", symbol_count);
                            tmp_arr+=c;
                            break;
                        default:
                            pass;
                    }
                }
            }
            else
            // We hit an delimiter
            {
                if (fookin_delim == 1 )
                {
                    // printf ("here\n");
                    continue;
                }
                if (new_section == 1)
                {
                    int sum = 0;
                    int i;

                    // Case when input file start with delimiter
                    if (tmp_arr[0] == '\0')
                    {
                        continue;
                    }

                    section_counter = std::stoi (tmp_arr);
                    if (section_counter == 0)
                    {
                        // printf ("Hitting zero\n");
                        cur_section++;
                        cur_section = cur_section % 3;
                        section_counter = 0;
                        // printf ("We are at section: %d\n", (int)cur_section);
                    }
                    else
                    {
                        // printf ("section_counter is: %d\n", section_counter);
                        new_section = 0;
                    }
                    tmp_arr.clear();
                    fookin_delim = 1;
                    continue;
                }
                if (fookin_delim == 0)
                {
                    // Specifics actions for passs2
                    if (cur_section == 1)
                    {
						/*if ( symbol_table.find(tmp_arr
						) != symbol_table.end() )
						{
                            le_use_list.push_back(tmp_arr);
                        }
                        else
                        {// the thing going in use_list is not defined
                            Use_Symbol rogue_symbol;
                            rogue_symbol.set_values(tmp_arr, false, cur_module);
                            undefined_list.push_back(rogue_symbol);
                        }*/
                        // Even if not in symbol table, still push to use_list;
                        // Input-8 occurs when too much in use_list.
                        Use_Symbol a_use_symbol;
                        a_use_symbol.set_values(tmp_arr, false, cur_module, false);
                        le_use_list.push_back(a_use_symbol);
                        /*
                        if (symbol_table.size() < le_use_list.size())
                        {
                            Use_Symbol overflow_symbol;
                            overflow_symbol.set_values(tmp_arr, false, cur_module, false);
                            unused_list.push_back(overflow_symbol);
                        }*/
                    }
                    if (cur_section == 2)
                    {
                        if (symbol_count % 2 == 0)
                        {
                            if (tmp_arr[0] == 'E')
                            {
                                tmp_prog_txt = 'E';
                            }
                            else if (tmp_arr[0] == 'R')
                            {
                                tmp_prog_txt = 'R';
                            }
							else if (tmp_arr[0] == 'A')
							{
								tmp_prog_txt = 'A';
							}
                            else if (tmp_arr[0] == 'I')
                            {
                                tmp_prog_txt = 'I';
                            }
                            else
                            {
                                // TODO
                            }
                            printf ("%03d: ", addr_counter);
                            addr_counter++;
                        }
                        else
                        {
                            int result;
                            if (tmp_prog_txt == 'E')
                            {
                                if (stoi(tmp_arr)%1000 >= le_use_list.size())
                                {
                                    printf ("%d", stoi(tmp_arr));
                                    cout << " Error: External address exceeds"
                                        " length of uselist; treated as immediate\n";
                                }
                                else
                                {
								string use_target = le_use_list[stoi(tmp_arr)%1000].symbol_name;
                                int to_add;
								if ( symbol_table.find(use_target
								) != symbol_table.end() )
								{
                                    le_use_list[stoi(tmp_arr)%1000].is_used = true;
                                    le_use_list[stoi(tmp_arr)%1000].is_defined = true;
                                	to_add = symbol_table[use_target].value;
                                    symbol_table[use_target].is_used = true;
                                    printf ("%d\n", (stoi(tmp_arr)/1000)*1000 +
                                        to_add);
                                }
                                else
                                {
                                    cout << (stoi(tmp_arr)/1000)*1000 <<
                                        " Error: " << use_target <<
                                        " is not defined; zero used\n";
                                    // just in case
                                    le_use_list[stoi(tmp_arr)%1000].is_defined = false;
                                    // to suppress the is_used Warning
                                    le_use_list[stoi(tmp_arr)%1000].is_used = true;
                                }
}
                            }
                            else if (tmp_prog_txt == 'R')
                            {
                                if (stoi(tmp_arr) % 1000 > section_counter)
                                {
                                    result = (stoi(tmp_arr) / 1000)*1000 +
                                        base_address;
                                    cout << result;
                                    cout << " Error: Relative address exceeds"
                                        " module size; zero used\n";
                                }
                                else
                                {
                                    result = stoi(tmp_arr) + base_address;
                                    printf ("%d\n", result);
                                }
                            }
                            else if (tmp_prog_txt == 'I')
                            {
                                result = stoi(tmp_arr);
                                printf ("%d\n", result);
                            }
                            else if (tmp_prog_txt == 'A')
                            {
                                result = stoi(tmp_arr);
                                // cout << "result:" << result << endl;
                                if ((result % 1000) > machine_size)
                                {
                                    printf ("%d ", (result/1000)*1000);
                                    cout << "Error: Absolute address exceeds"
                                        " machine size; zero used\n";
                                }
                                else
                                {
                                    printf ("%d\n", result);
                                }
                            }
                            else
                            {
                                //TODO
                            }
                        }
                    }
                    // flush
                    tmp_arr.clear();

                    if (symbol_count!= -1)
                {
                        symbol_count--;

                        // moving on to the next section
                        if (symbol_count == 0)
                        {
                            if (cur_section == 2)
                            {
                                for(vector<Use_Symbol>::size_type i = 0; i != le_use_list.size(); i++) {
                                    if (le_use_list[i].is_used == false)
                                    {
                                    cout << "Warning: Module " << le_use_list[i].defined_module <<
                                        ": "<< le_use_list[i].symbol_name <<" appeared in the uselist"
                                        " but was not actually used\n";
                                    }
                                }
                                // new use list
                                le_use_list.clear();
                            }
                            symbol_count = -1;
                            cur_section++;
                            cur_section = cur_section % 3;
                            new_section = 1;
                            module_start = 0;
                            if (cur_section == 0)
                            {
                                cur_module++;

                            }
                        }
                    }
                    else
                    {
                        expect_symbol = 1;
                    }
                }
                fookin_delim = 1;
                // continue;
            }
        }
        if (module_start != 0 || cur_section != 0)
        {
            printf ("Error: File ended before finishing a module\n");
        }
        if (symbol_count != -1 || expect_symbol == 1)
        {
            // TODO
            // Find where the fuck the wrong is.
            printf ("Input finished before sufficient symbols!\n");
        }
        fclose(ifp);

        // Warning output
        /*
        for(vector<Use_Symbol>::size_type i = 0; i != unused_list.size(); i++) {
            cout << "Warning: Module " << unused_list[i].defined_module <<
                ": "<< unused_list[i].symbol_name <<" appeared in the uselist"
                " but was not actually used\n";
        }*/
        cout << endl << endl;
	    for( std::map<string, Def_Symbol>::iterator iter = symbol_table.begin();
     	iter != symbol_table.end();
     	++iter )
        {
            if (iter->second.is_used == false)
            {
                cout << "Warning: Module " << iter->second.defined_module <<
                    ": "<< iter->first <<" was defined but never used\n";
            }
        }
    }
    else
    {
        fprintf (stderr, "Can't open file %s\n", input_file);
        exit (1);
    }

    ofp = fopen (output_file, "w");
    if (ofp == NULL)
    {
        fprintf (stderr, "can't write to file\n");
        exit (1);
    }
}

