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
    }
    else
    {
        fprintf (stderr, "Can't open file %s\n", input_file);
        exit (1);
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

        // tmp array
        string tmp_arr;

        // use list
        std:vector<string> le_use_list;

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
                        le_use_list.push_back(tmp_arr);
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
								string use_target = le_use_list[stoi(tmp_arr)%1000];
                                int to_add;
								if ( symbol_table.find(use_target
								) != symbol_table.end() )
								{
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
                                }
                            }
                            else if (tmp_prog_txt == 'R')
                            {
                                result = stoi(tmp_arr) + base_address;
                                printf ("%d\n", result);
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
                            if (cur_section == 1)
                            {
                                // new use list
                                le_use_list.clear();
                            }
                            symbol_count = -1;
                            cur_section++;
                            cur_section = cur_section % 3;
                            new_section = 1;
                            module_start = 0;
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
        cout << endl;
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

