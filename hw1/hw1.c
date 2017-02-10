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
    int new_line = 0;
    int new_module = 0;
    typedef enum {def_list, use_list, prog_txt} stage;

    ifp = fopen(input_file, mode);
    if (ifp) {
        stage pass1_stage = def_list;
        while ((c = getc(ifp)) != EOF)
            if (new_line == 1)
            {
                if (c == 0)
                {
                    new_module == 1;
                }
            }
            if (c == '\t' || c == '\n')
            {
                // new stage
                new_line = 1;
            }
            else
            {
            putchar(c);
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

