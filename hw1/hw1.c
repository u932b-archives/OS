# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>

#define pass (void)0

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
    char *mode = "r";
    char output_file[] = "out.list";
    char c;
    typedef enum {def_list, use_list, prog_txt} section;
    int max_arr_size = 17;

    typedef struct symbol
    {
       char* key;
       int value;
    } a_symbol;
    int t_size = 0;

    a_symbol* symbol_table = (a_symbol*) malloc(sizeof(a_symbol) * t_size);

    // a_symbol.key = 1;
    // a_symbol.value = "foo";

    ifp = fopen(input_file, mode);
    if (ifp)
    {
        // flags
        int new_section = 1;
        int fookin_delim = 0;
        int expect_symbol = 0;
        int module_start = 1;

        section pass1_section = def_list;

        // counters
        int section_counter = 0;
        int symbol_count = -1;
        int base_address = 0;

        // tmp array
        char* tmp_arr;
        int combo = 0;
        tmp_arr = malloc((combo+1)*sizeof(c));
        tmp_arr[combo] = '\0';


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
                    // printf ("We are at section: %d\n", (int)pass1_section);
                    combo++;
                    tmp_arr = realloc(tmp_arr, (combo+1)*sizeof(c));
                    tmp_arr[combo-1] = c;
                    tmp_arr[combo] = '\0';
                }
                else
                {
                    switch (pass1_section)
                    {
                        case def_list:
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
                            combo++;
                            check_arr_size(combo, max_arr_size);
                            tmp_arr = realloc(tmp_arr, (combo+1)*sizeof(c));
                            tmp_arr[combo-1] = c;
                            tmp_arr[combo] = '\0';
                            // printf ("building up the tmp_arr: %s, combo is "
                            //         "currently: %d\n", tmp_arr, combo);
                            break;
                        case use_list:
                            // printf ("current c: %c\n", c);
                            // printf ("section_counter is: %d\n", section_counter);
                            // printf ("symbol_count is: %d\n", symbol_count);
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter;
                            }
                            combo++;
                            check_arr_size(combo, max_arr_size);
                            tmp_arr = realloc(tmp_arr, (combo+1)*sizeof(c));
                            tmp_arr[combo-1] = c;
                            tmp_arr[combo] = '\0';
                            // printf ("building up the tmp_arr: %s, combo is "
                            //         "currently: %d\n", tmp_arr, combo);
                            printf ("use_list to symbol_table3: %s\n", symbol_table[0].key);
                            break;
                        case prog_txt:
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter * 2;
                                base_address += section_counter;
                            }
                            // printf ("symbol_count is: %d\n", symbol_count);
                            combo++;
                            check_arr_size(combo, max_arr_size);
                            tmp_arr = realloc(tmp_arr, (combo+1)*sizeof(c));
                            tmp_arr[combo-1] = c;
                            tmp_arr[combo] = '\0';
                            // printf ("building up the tmp_arr: %s, combo is "
                            //         "currently: %d\n", tmp_arr, combo);
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
                        pass1_section++;
                        pass1_section = pass1_section % 3;
                        section_counter = 0;
                        // printf ("We are at section: %d\n", (int)pass1_section);
                    }
                    else
                    {
                        // section_counter = (int)sum - '0';
                        // printf ("section_counter is: %d\n", section_counter);
                        new_section = 0;
                    }
                    combo = 0;
                    free(tmp_arr);
                    tmp_arr = malloc((combo+1)*sizeof(c));
                    tmp_arr[1] = '\0';
                    fookin_delim = 1;
                    continue;
                }
                if (fookin_delim == 0)
                {
                    // Gotta write to symbol_table
                    if (pass1_section == 0)
                    {
                        if (symbol_count % 2 == 0)
                        {
                            t_size++;
                            symbol_table = (a_symbol*) realloc(symbol_table,
                                t_size*sizeof(a_symbol));
                            // symbol_table[t_size-1].key = malloc(sizeof(tmp_arr));
                            printf ("t_size: %d\n", t_size);
                            // symbol_table[t_size-1].key = tmp_arr;
                            symbol_table[0].key = tmp_arr;
                            // printf ("%s\n", tmp_arr);
                            printf ("write key to symbol_table2: %s\n", symbol_table[t_size-1].key);
                            printf ("write key to symbol_table3: %s\n", symbol_table[0].key);
                        }
                        else if (symbol_count % 2 == 1)
                        {
                            // somehow sscanf not working
                            // int i;
                            // printf ("sscanf: %d\n", sscanf(tmp_arr, "%d",&i));
                            printf (" yo keypair in symbol_table1: %s\n",
                            symbol_table[0].key);
                            symbol_table[t_size-1].value = base_address +
                                atoi(tmp_arr);
                            printf ("write val to symbol_table: %s\n", tmp_arr);
                            printf ("keypair in symbol_table1: %d\n",
                            symbol_table[0].value);
                        }
                    }

                    // flush
                    combo = 0;
                    // printf ("%s\n", tmp_arr);
                    free(tmp_arr);
                    tmp_arr = malloc((combo+1)*sizeof(c));
                    tmp_arr[1] = '\0';

                    if (symbol_count!= -1)
                    {
                        symbol_count--;
                        if (symbol_count == 0)
                        {
                            // printf ("moving on to the next section.\n");
                            symbol_count = -1;
                            pass1_section++;
                            pass1_section = pass1_section % 3;
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
        if (module_start != 0 || pass1_section != 0)
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

    int i;
    for (i = 0; i < t_size; i++)
    {
        printf ("keypair in symbol_table: %s, %d\n",
        symbol_table[i].key, symbol_table[i].value);
    }
    ofp = fopen (output_file, "w");
    if (ofp == NULL)
    {
        fprintf (stderr, "can't write to file\n");
        exit (1);
    }
}

