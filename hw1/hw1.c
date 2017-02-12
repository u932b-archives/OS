# include <stdio.h>
# include <string.h>
# include <stdlib.h>

#define pass (void)0

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

    ifp = fopen(input_file, mode);
    if (ifp)
    {
        int new_section = 1;
        int fookin_delim = 0;
        section pass1_section = def_list;
        int section_counter = 0;
        int symbol_count = -1;
        char* tmp_arr;
        int combo = 0;
        tmp_arr = malloc((combo+1)*sizeof(c));
        while ((c = getc(ifp)) != EOF)
        {
            if (c != '\t' && c != '\n' && c != ' ')
            {
                fookin_delim = 0;
                if (new_section == 1)
                {
                    // This would print as many times as the number of char
                    // in the section_counter
                    printf ("We are at section: %d\n", (int)pass1_section);
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
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                // This many symbols before next section
                                symbol_count = section_counter*2;
                            }
                            combo++;
                            tmp_arr = realloc(tmp_arr, (combo+1)*sizeof(c));
                            tmp_arr[combo-1] = c;
                            tmp_arr[combo] = '\0';
                            // printf ("building up the tmp_arr: %s, combo is "
                            //         "currently: %d\n", tmp_arr, combo);
                            break;
                        case use_list:
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter;
                            }
                            combo++;
                            tmp_arr = realloc(tmp_arr, (combo+1)*sizeof(c));
                            tmp_arr[combo-1] = c;
                            tmp_arr[combo] = '\0';
                            // printf ("building up the tmp_arr: %s, combo is "
                            //         "currently: %d\n", tmp_arr, combo);
                            break;
                        case prog_txt:
                            if (section_counter > 0 && symbol_count == -1 )
                            {
                                symbol_count = section_counter * 2;
                            }
                            combo++;
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
                    /*
                    for (i = 0; i < combo+1; ++i) {
                        sum |= tmp_arr[i];
                    }
                    if (sum == 0)
                    {
                    */

                    sscanf(tmp_arr, "%d", &section_counter);
                    if (section_counter == 0)
                    {
                        printf ("Hitting zero\n");
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
                    // flush
                    combo = 0;
                    printf ("%s\n", tmp_arr);
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
                        }
                    }
                }
                fookin_delim = 1;
                // continue;
            }
        }
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

