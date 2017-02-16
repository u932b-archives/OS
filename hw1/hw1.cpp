# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <map>
# include <String>
# include <iostream>
#include <set>
using namespace std;

#define pass (void)0
int max_arr_size = 17;
int machine_size = 512;

void check_arr_size(int combo, int max_arr_size)
{
    if (combo <= max_arr_size)
    {
        return;
    }
    else
    {
        printf ("Error: maximum arr size reached\n");
    }
}

int equal( char array1[] , char array2[] )
{
	int i;
    for ( i = 0; array1[i] && array2[i]; ++i )
    {
        if ( array1[i] != array2[i] )
        {
            return (0);
        }
    }
    return (1);
}

void add_char_to_tmparr(int* combo, char c, char* tmp_arr)
{
    // *combo = *combo + 1;
    (*combo)++;
    check_arr_size(*combo, max_arr_size);
    tmp_arr = (char*)realloc(tmp_arr, (*combo+1)*sizeof(c));
    tmp_arr[*combo-1] = c;
    tmp_arr[*combo] = '\0';
    // printf ("building up the tmp_arr: %s, combo is "
    //         "currently: %d\n", tmp_arr, combo);
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

    // char *input_file = strcpy(input_file, argv[1]);
    char *input_file = argv[1];
    FILE *ifp, *ofp;
    // char *mode = "r";
    char output_file[] = "out.list";
    char c;
    // typedef enum {def_list, use_list, prog_txt} section;
    // const int* section[3] = {0, 1, 2};
    int cur_section = 0;

    // typedef struct symbol
    // {
    //    char* key;
    //    int value;
    // } a_symbol;
    // int t_size = 0;

    // // symbol table
    // a_symbol* symbol_table = (a_symbol*) malloc(sizeof(a_symbol) * 1);
    // // a_symbol* symbol_table = (a_symbol*) malloc(sizeof(a_symbol) * t_size);
    map<string, int> symbol_table;
    std::set<string> duplicates;


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

        // tmp array
        char* tmp_arr;
        int combo = 0;
        tmp_arr = (char*)malloc((combo+1)*sizeof(c));
        tmp_arr[combo] = '\0';


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
                    add_char_to_tmparr(&combo, c, tmp_arr);
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
                            add_char_to_tmparr(&combo, c, tmp_arr);
                            break;

                        case 1:
                            // printf ("current c: %c\n", c);
                            // printf ("section_counter is: %d\n", section_counter);
                            // printf ("symbol_count is: %d\n", symbol_count);
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter;
                            }
                            add_char_to_tmparr(&combo, c, tmp_arr);
                            break;

                        case 2:
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter * 2;
                                base_address += section_counter;
                            }
                            // printf ("symbol_count is: %d\n", symbol_count);
                            add_char_to_tmparr(&combo, c, tmp_arr);
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

                    sscanf(tmp_arr, "%d", &section_counter);
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
                        // section_counter = (int)sum - '0';
                        // printf ("section_counter is: %d\n", section_counter);
                        new_section = 0;
                    }
                    combo = 0;
                    free(tmp_arr);
                    tmp_arr = (char*)malloc((combo+1)*sizeof(c));
                    tmp_arr[1] = '\0';
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
                            // symbol_table[tmp_arr] = -1;
                            // strncpy(map_key, tmp_arr, sizeof(tmp_arr));
                            map_key = tmp_arr;
                            // printf ("%s\n", map_key);
                            // printf ("written key to symbol_table: %s\n",
                            //        symbol_table[t_size-1].key);
                            // printf ("%s\n", tmp_arr);
                        }
                        else if (symbol_count % 2 == 1)
                        {
                            // somehow sscanf not working
                            // int i;
                            // printf ("sscanf: %d\n", sscanf(tmp_arr, "%d",&i));
                            if (symbol_table.find(map_key) == symbol_table.end())
                            {
                                symbol_table[map_key] = base_address + atoi(tmp_arr);
                            }
                            else
                            {
                                duplicates.insert(map_key);
                            }
                            /*symbol_table[t_size-1].value = base_address +
                                atoi(tmp_arr);*/
                            // printf ("write val to symbol_table: %s\n", tmp_arr);
                            // printf ("key in symbol_table1: %s\n",
                            // symbol_table[t_size-1].key);
                            // printf ("t_size: %d\n", t_size);
                            // printf ("value in symbol_table: %d\n",
                            // symbol_table[t_size-1].value);
                        }
                    }

                    // flush
                    combo = 0;
                    free(tmp_arr);
                    tmp_arr = (char*)malloc((combo+1)*sizeof(c));
                    tmp_arr[1] = '\0';

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
        // free(symbol_table);
        free(tmp_arr);
        fclose(ifp);
    }
    else
    {
        fprintf (stderr, "Can't open file %s\n", input_file);
        exit (1);
    }


    printf ("Symbol Table\n");
	for( std::map<string, int>::iterator iter = symbol_table.begin();
     	iter != symbol_table.end();
     	++iter )
	{
        if (duplicates.find(iter->first) == duplicates.end())
        {
            std::cout << iter->first << "=" << iter->second << "\n";
        }
        else
        {
            std::cout << iter->first << "=" << iter->second <<
            " Error: This variable is multiple times defined; first value used"
            << "\n";
        }
	}
	/*
    int i;
    for (i = 0; i < t_size; i++)
    {
        printf ("%s=%d\n",
        symbol_table[i].key, symbol_table[i].value);
        // free(symbol_table[i].key);
    }*/
    // free(symbol_table);


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
        char* tmp_arr;
        int combo = 0;
        tmp_arr = (char*)malloc((combo+1)*sizeof(c));
        tmp_arr[combo] = '\0';

        // use list
        char** le_use_list;
        int use_count = 0;
        le_use_list = (char**)malloc((use_count)*sizeof(char*));

        // tmp storage
        char tmp_prog_txt = 'N';

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
                    add_char_to_tmparr(&combo, c, tmp_arr);
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
                            add_char_to_tmparr(&combo, c, tmp_arr);
                            break;
                        case 1:
                            // printf ("current c: %c\n", c);
                            // printf ("section_counter is: %d\n", section_counter);
                            // printf ("symbol_count is: %d\n", symbol_count);
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter;
                            }
                            add_char_to_tmparr(&combo, c, tmp_arr);
                            break;
                        case 2:
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter * 2;
                                base_address += prev_section_counter;
                                prev_section_counter = section_counter;
                            }
                            // printf ("symbol_count is: %d\n", symbol_count);
                            add_char_to_tmparr(&combo, c, tmp_arr);
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

                    sscanf(tmp_arr, "%d", &section_counter);
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
                        // section_counter = (int)sum - '0';
                        // printf ("section_counter is: %d\n", section_counter);
                        new_section = 0;
                    }
                    combo = 0;
                    free(tmp_arr);
                    tmp_arr = (char*)malloc((combo+1)*sizeof(c));
                    tmp_arr[1] = '\0';
                    fookin_delim = 1;
                    continue;
                }
                if (fookin_delim == 0)
                {
                    // Specifics actions for passs2
                    if (cur_section == 1)
                    {
                        use_count++;
                        le_use_list = (char**)realloc(le_use_list,
                                (use_count)*sizeof(char*));
                        le_use_list[use_count-1] = (char*)malloc(sizeof(tmp_arr));
                        strncpy(le_use_list[use_count-1], tmp_arr, sizeof(tmp_arr));
                        // free(tmp_arr);
                    }
                    if (cur_section == 2)
                    {
                        if (symbol_count % 2 == 0)
                        {
                            if (tmp_arr[0] == 'E')
                            {
                                // printf ("Hey it's an E!\n");
                                tmp_prog_txt = 'E';
                            }
                            else if (tmp_arr[0] == 'R')
                            {
                                // printf ("Hey it's an R!\n");
                                tmp_prog_txt = 'R';
                            }
							else
							{
								tmp_prog_txt = 'N';
							}
                            printf ("%03d: ", addr_counter);
                            addr_counter++;
                        }
                        else
                        {
                            if (tmp_prog_txt == 'E')
                            {
								char *use_target = le_use_list[atoi(tmp_arr)%1000];
                                // printf ("Used to be: %s\n", tmp_arr);
                                // printf ("here.\n");
                                // printf ("Test: %s\n", le_use_list[0]);
                                // printf ("To add: %s\n",
                                //         le_use_list[atoi(tmp_arr)%1000]);
                                int i;
                                int to_add;
                                // use_count is unset
                                // printf ("QQQ %d\n", use_count);
								// printf ("QQ %s\n", use_target);
								if ( symbol_table.find(use_target
								) != symbol_table.end() )
								{
                                        // printf ("key to print:%s\n",
                                        //         symbol_table[i].key);
                                        // printf ("val to print:%d\n",
                                        //         symbol_table[i].value);

                                	to_add = symbol_table[use_target];
                                }
                                printf ("%d\n", (atoi(tmp_arr)/1000)*1000 +
                                        to_add);

                            }
                            else if (tmp_prog_txt == 'R')
                            {
                                printf ("%d\n", atoi(tmp_arr) + base_address);
                            }
                            else
                            {
                                printf ("%s\n", tmp_arr);
                            }
                        }
                    }
                    // flush
                    combo = 0;
                    free(tmp_arr);
                    tmp_arr = (char*)malloc((combo+1)*sizeof(c));
                    tmp_arr[1] = '\0';

                    if (symbol_count!= -1)
                    {
                        symbol_count--;
                        if (symbol_count == 0)
                        {
                            // printf ("%d ", fookin_delim);
                            // printf ("cur_section:%d\n", cur_section);
                            // printf ("moving on to the next section.\n");
                            // printf ("c: %c", c);
                            if (cur_section == 1)
                            {
                                // new use list
                                // int i;
                                // for (i=0; i < use_count; i++)
                                // {
                                //     free(le_use_list[i]);
                                // }
                                use_count = 0;
                                free(le_use_list);
                                le_use_list = (char**)malloc((use_count)*sizeof(c));
                                // printf ("Test: %s\n", le_use_list[0]);
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
        // free(symbol_table);
        fclose(ifp);
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

