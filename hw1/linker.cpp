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

inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

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

struct location
{
    int line_num;
    int offset;
    bool is_delim;
} last_symbol_location, def_count_loc;

class Use_Count {
    public:
        int defined_module;
        int count;
        location use_count_loc;
        void set_values (int, int);
};

void Use_Count::set_values (int module, int val)
{
    defined_module = module;
    count = val;
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
    FILE *ifp;
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


        // counters
        int section_counter = 0;
        int symbol_count = -1;
        int base_address = 0;
        int cur_module = 1;

        string tmp_arr;

        string map_key;

        int c_count = -1;
        int line_num = 1;

        int num_instr = 0;

        vector <Use_Count> use_count_vec;

        while ((c = getc(ifp)) != EOF)
        {
            c_count++;

            if (c != '\t' && c != '\n' && c != ' ')
            {
                // cout << c << " ";
                fookin_delim = 0;
                expect_symbol = 0;

                if (new_section == 1)
                {
                    // This would print as many times as the number of char
                    // in the section_counter
                    tmp_arr+=c;
                }
                else
                {
                    switch (cur_section)
                    {
                        case 0:
                            module_start = 1;
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                // This many symbols before next section
                                symbol_count = section_counter*2;
                            }
                            if (symbol_count % 2 == 1 && !isdigit(c))
                            {
                                // Should be a digit instead
                                cout << "Parse Error line " << line_num <<
                                    " offset " << c_count+1 << ": NUM_EXPECTED\n";
                                exit (1);
                            }

                            tmp_arr+=c;
                            break;

                        case 1:
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
                    last_symbol_location.line_num = line_num;
                    last_symbol_location.offset = c_count;
                    last_symbol_location.is_delim = true;
                    if (c == '\n')
                    {
                        line_num++;
                        c_count = -1;
                    }
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
                    if (cur_section == 0)
                    {
                        if (section_counter > 16)
                        {
                            cout << "Parse Error line " <<
                            line_num << " offset "
                            << c_count - tmp_arr.length() + 1 <<
                            ": TO_MANY_DEF_IN_MODULE\n";
                            exit (1);
                        }
                    }
                    if (cur_section == 1)
                    {
                        // Input-16
                        if (section_counter > 16)
                        {
                            cout << "Parse Error line " <<
                            line_num << " offset "
                            << c_count - tmp_arr.length() + 1 <<
                            ": TO_MANY_USE_IN_MODULE\n";
                            exit (1);
                        }
                    }
                    // Input-17
                    if (cur_section == 2)
                    {
                        num_instr += section_counter;
                        if (num_instr > 512)
                        {
                            cout << "Parse Error line " << line_num <<
                                " offset " << c_count - tmp_arr.length() + 1
                                << ": TO_MANY_INSTR\n";
                            exit (1);
                        }
                    }

                    if (section_counter == 0)
                    {
                        // Zeros
                        cur_section++;
                        cur_section = cur_section % 3;
                        section_counter = 0;
                    }
                    else
                    {
                        new_section = 0;
                    }

                    if (cur_section == 0)
                    {
                            def_count_loc.line_num = line_num;
                            def_count_loc.offset = c_count - 1;
                    }
                    last_symbol_location.line_num = line_num;
                    last_symbol_location.offset = c_count-1;
                    tmp_arr.clear();
                    fookin_delim = 1;
                    // Untested Fix
                    if (c != '\n')
                    {
                        continue;
                    }
                }
                if (fookin_delim == 0)
                {
                    // Gotta write to symbol_table
                    if (cur_section == 0)
                    {
                        if (symbol_count % 2 == 0)
                        {
                            if (tmp_arr.length() > 16)
                            {
                                cout << "Parse Error line " << line_num << " offset "
                                    << c_count - tmp_arr.length() + 1 <<": SYM_TOO_LONG\n";
                                exit (1);
                            }
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
                    else if (cur_section == 1)
                    {
                        if (isInteger(tmp_arr))
                        {
                            // Not used?
                            cout << "Parse Error line " <<
                                line_num <<
                                " offset " << c_count - tmp_arr.length() +1
                                << ": SYM_EXPECTED\n";
                            exit (1);
                        }
                    }
                    else if (cur_section == 2)
                    {
                        // check type
                        if (symbol_count % 2 == 0)
                        {
                            if (isInteger(tmp_arr))
                            {
                                // Not used?
                                cout << "Parse Error line " <<
                                    line_num <<
                                    " offset " << c_count + 1
                                    << ": SYM_EXPECTED\n";
                                exit (1);
                            }
                        }
                        else
                        {
                            if (!isInteger(tmp_arr))
                            {
                                // Not used?
                                cout << "Parse Error line " <<
                                    line_num <<
                                    " offset " << c_count + 1
                                    << ": ADDR_EXPECTED\n";
                                exit (1);
                            }

                        }
                    }

                    // Update last_symbol_location
                    last_symbol_location.line_num = line_num;
                    last_symbol_location.offset = c_count-1;

                    // flush
                    tmp_arr.clear();

                    if (symbol_count!= -1)
                    {
                        symbol_count--;
                        if (symbol_count == 0)
                        {
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
                if (c == '\n')
                {
                    line_num++;
                    c_count = -1;
                }
                // continue;
            }
        }
        if (module_start != 0 || cur_section != 0)
        {
            // This should just means the pass was not ended clean.
            if (cur_section == 0)
            {
                if (symbol_count != -1)
                {
                    // Something else should be printed here.
                    pass;
                }
                else
                {
                    // Input-13, 18
                    // Here it is expect two offset after the previous symbol
                    int offset;
                    if (last_symbol_location.is_delim == true)
                    {// case for Input-18
                        offset = last_symbol_location.offset + 1;
                    }
                    else
                    {
                        offset = last_symbol_location.offset + 2;
                    }
                    cout << "Parse Error line " <<
                        last_symbol_location.line_num <<
                        // line_num <<
                        " offset " << offset
                        << ": SYM_EXPECTED\n";
                    exit (1);
                }
            }

            if (cur_section == 1)
            {
                if (isInteger(tmp_arr))
                {
                    // Not used?
                    cout << "Parse Error line " <<
                        line_num <<
                        " offset " << c_count - tmp_arr.length() + 1
                        << ": SYM_EXPECTED\n";
                    exit (1);
                }
            }
            // for input 2 where the pass ends confusingly
            if (cur_section == 2)
            {
                 if (symbol_count % 2 == 0)
                 {
                     if (isInteger(tmp_arr))
                     {
                         // Not used
                         cout << "Parse Error line " <<
                             line_num <<
                             " offset " << c_count + 1
                             << ": SYM_EXPECTED\n";
                         exit (1);
                     }
                 }
                 else
                 {
                     if (!isInteger(tmp_arr))
                     {
                         // Expected two offsets after the previous
                         cout << "Parse Error line " <<
                             last_symbol_location.line_num <<
                             " offset " << last_symbol_location.offset + 2
                             << ": ADDR_EXPECTED\n";
                         exit (1);
                     }

                 }
            }
            printf ("Error: File ended before finishing a module\n");
            exit (1);
        }
        if (symbol_count != -1 || expect_symbol == 1)
        {
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

        string tmp_arr;

        // use list
        vector<Use_Symbol> le_use_list;
        char tmp_prog_txt;

        printf ("\nMemory Map\n");
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
                    tmp_arr+=c;
                }
                else
                {
                    switch (cur_section)
                    {
                        case 0:
                            module_start = 1;
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                // This many symbols before next section
                                symbol_count = section_counter*2;
                            }
                            tmp_arr+=c;
                            break;
                        case 1:
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
                                base_address += prev_section_counter;
                                prev_section_counter = section_counter;
                            }
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
                        // Hitting zero
                        cur_section++;
                        cur_section = cur_section % 3;
                        section_counter = 0;
                    }
                    else
                    {
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
                        Use_Symbol a_use_symbol;
                        a_use_symbol.set_values(tmp_arr, false, cur_module, false);
                        le_use_list.push_back(a_use_symbol);
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
                            if (stoi(tmp_arr) > 9999)
                            {
                                if (tmp_prog_txt == 'R')
                                {
                                    cout << "9999 Error: Illegal opcode; treated as 9999\n";
                                }
                                else
                                {
                                    cout << "9999 Error: Illegal immediate value; treated as 9999\n";
                                }
                            }
                            else if (tmp_prog_txt == 'E')
                            {
                                if (stoi(tmp_arr)%1000 >= le_use_list.size())
                                {
                                    printf ("%04d", stoi(tmp_arr));
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
                                    printf ("%04d\n", (stoi(tmp_arr)/1000)*1000 +
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
                                    printf ("%04d\n", result);
                                }
                            }
                            else if (tmp_prog_txt == 'I')
                            {
                                result = stoi(tmp_arr);
                                printf ("%04d\n", result);
                            }
                            else if (tmp_prog_txt == 'A')
                            {
                                result = stoi(tmp_arr);
                                if ((result % 1000) > machine_size)
                                {
                                    printf ("%04d ", (result/1000)*1000);
                                    cout << "Error: Absolute address exceeds"
                                        " machine size; zero used\n";
                                }
                                else
                                {
                                    printf ("%04d\n", result);
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
}

