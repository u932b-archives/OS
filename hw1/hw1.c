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
    int new_section = 1;

    ifp = fopen(input_file, mode);
    if (ifp)
    {
        section pass1_section = def_list;
        int section_counter = 0;
        int defpair_count = -1;
        char* tmp_arr;
        int combo = 0;
        tmp_arr = malloc((combo+1)*sizeof(c));
        while ((c = getc(ifp)) != EOF)
        {
            if (new_section == 1)
            {
                // If the section_counter is 0, it means we are going into a new
                // module
                if (c == 0)
                {
                    pass1_section = def_list;
                    new_section = 0;
                }
                else
                {
                    section_counter = (int)c - '0';
                    new_section = 0;
                }
                continue;
            }
            if (c != '\t' && c != '\n' && c != ' ')
            {

                switch (pass1_section)
                {
                    case def_list:
                        printf ("section_counter is: %d\n", section_counter);
                        if (section_counter > 0 && defpair_count == -1 )
                        {
                            // I have to deal with this many symbols before next
                            // section
                            defpair_count = section_counter*2;
                        }
                        if (defpair_count > 0)
                        {
                            printf ("defpair_count is: %d\n", defpair_count);
                            combo++;
                            // printf ("c is now: %c\n", c);
                            tmp_arr = realloc(tmp_arr, (combo+1)*sizeof(c));
                            tmp_arr[combo-1] = c;
                            tmp_arr[combo] = '\0';
                            // printf ("building up the tmp_arr: %s, combo is "
                            //         "currently: %d\n", tmp_arr, combo);
                            // putchar(c);
                            defpair_count--;
                        int i;
                        }
                        else
                        {
                            printf ("moving on to the next section.\n");
                            pass1_section = use_list;
                        }
                    case use_list:
                        pass;
                    case prog_txt:
                        pass;
                    default:
                        pass;
                }
            }
            else
            {
                combo = 0;
                // printf ("WTF\n");
                printf ("Delimiter. tmp_arr is: %s\n", tmp_arr);
                free(tmp_arr);
                tmp_arr = malloc((combo+1)*sizeof(c));
                tmp_arr[1] = '\0';
            }
        }
        fclose(ifp);
    }
    else
    {
        fprintf (stderr, "Can't open file %s\n", input_file);
        exit (1);
    }/*
    else
    {
        // char buff[255];
        int a, b;
        char* string1;

        fscanf(ifp, "%d %s %d", &a, &string1, &b);
        printf ("%d %d\n", a, b);
        printf ("%s\n", string1);
    }*/
    // fclose(ifp);

    ofp = fopen (output_file, "w");
    if (ofp == NULL)
    {
        fprintf (stderr, "can't write to file\n");
        exit (1);
    }
}

