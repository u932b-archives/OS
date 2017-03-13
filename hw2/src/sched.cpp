# include <iostream>
#include <fstream>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

class Process
{
	public:
		Process(int at, int tc, int cb, int io, string stat);
		int getAT(void);
		int getTC(void);
		int getCB(void);
		int getIO(void);
		string getState(void);
	private:
		int AT; // Arrival Time
		int TC; // Total CPU Time
		int CB; // CPU Burst
		int IO; // IO Burst
		string state;
};

int Process::getAT( void ) {
   return AT;
}

int Process::getTC( void ) {
   return TC;
}

int Process::getCB( void ) {
   return CB;
}

int Process::getIO( void ) {
   return IO;
}

string Process::getState( void ) {
   return state;
}

Process::Process(int at, int tc, int cb, int io, string stat)
{
	AT = at;
	TC = tc;
	CB = cb;
	IO = io;
	state = stat;
}
/*
int myrandom(int burst)
{
	return 1+(randvals[ofs] % burst);
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
	int index;

    while ((c = getopt (argc, argv, "vs:")) != -1)
    {
        switch (c)
        {
            case 'v':
                cout << "got v" << endl;
                break;
            case 's':
                cout << "got s" << endl;
                svalue = optarg;
                break;
			case '?':
                cout << "got ?" << endl;
       			if (optopt == 'c')
       			   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
       			else if (isprint (optopt))
       			  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
       			else
       			  fprintf (stderr,
       			           "Unknown option character `\\x%x'.\n",
       			           optopt);
       			return -1;
            default:
                abort ();

        }
    }
	ifstream inFile;
    ifstream randFile;
	inFile.open(argv[optind-1]);
	randFile.open(argv[optind]);
	string line;
	vector <Process> DES;
    while (!inFile.eof())
    {
        getline(inFile, line);
        if (!line.empty())
        {
            cout << "Confirmation of data reading: " << line << endl;
			string buf; // Have a buffer string
    		stringstream ss(line); // Insert the string into a stream
    		vector<string> tokens; // Create vector to hold our words
    		while (ss >> buf)
        		tokens.push_back(buf);
			Process newProcess(stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2])
			, stoi(tokens[3]), "CREATE");

			DES.push_back(newProcess);

        }
    }
	for (int i=0; i < DES.size(); i++)
	{
		cout << DES[i].getState() << endl;
	}

	inFile.close();

}
