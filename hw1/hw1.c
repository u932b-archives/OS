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

    ifp = fopen(input_file, mode);
    if (ifp == NULL)
    {
        fprintf (stderr, "Can't open file %s\n", input_file);
        exit (1);
    }
    else
    {
        char buff[255];
        fscanf(ifp, "%s", buff);
        printf("1 : %s\n", buff );
        fgets(buff, 255, (FILE*)ifp);
        printf("2: %s\n", buff );
    }
    // fclose(ifp);

    ofp = fopen (output_file, "w");
    if (ofp == NULL)
    {
        fprintf (stderr, "can't write to file\n");
        exit (1);
    }
}

