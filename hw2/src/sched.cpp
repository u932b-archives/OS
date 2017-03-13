# include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc > 1) {
      // This will print the first argument passed to your program
    }
	ifstream inFile;
	inFile.open(argv[1]);
	string line;
    while (!inFile.eof())
    {
        getline(inFile, line);
        if (!line.empty())
        {
            cout << "Confirmation of data reading: " << line << endl;
        }
    }
	inFile.close();

}
