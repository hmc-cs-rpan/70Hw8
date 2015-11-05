/**
 * \file message-passer.cpp
 * \author CS70 Provided Code
 */

#include <iostream>
#include <fstream>
#include <random>
#include "chunkystring.hpp"
#include "noisy-transmission.hpp"

using namespace std;

/**
 * \brief Option Processing
 * \details
 *   Sets various configuration variables by modifying variables passed
 *   by reference.
 *
 *   Will return with an exit error of 2 if receives a usage problem.
 *
 * \param options       Input of options from command line.
 * \param filename      Name of file to read original message from
 * \param noiseLevel    Likelihood of a character being modified
 */
void processOptions(list<string> options,
		    string& filename,
                    float& noiseLevel)
{

    // Takes two things off the list at a time. The first one is a flag, the
    // second is the argument to the flag.
    string flag;
    string value;
    while (!options.empty()) {
    	// Command line arguments are parsed in flag/value pairs
        flag = options.front();
        options.pop_front();

        if (options.empty()) {
            cerr << "Empty argument" << endl;
            exit(2);
        };

	value = options.front();
	options.pop_front();

	if (flag == "-n" || flag == "--noise") {
	    noiseLevel = stof(value);
        } else if (flag == "-f" || flag == "--filename") {
            filename = value;
        } else {
            cerr << "Unrecognized option: " << flag << endl;
            cerr << "Usage: ./messagePasser -n noise -f filename" << endl;
            exit(2);
        }
    }

    if (fileName.empty()) {
        cerr << "Filename not specfied" << endl;
        exit(2);
    }
}



int main(int argc, const char* argv[])
{
    float noiseLevel = 0;
    string fileName;

    // Construct a list of options that goes from the 2nd element of argv to
    // the last one. We don't care about the first element because it's just
    // the name of the program
    list<string> options(argv + 1, argv + argc);
    processOptions(options, fileName, noiseLevel);

    ifstream fileReader( fileName );

    if (!fileReader.is_open()) {
        // The file could not be opened
        cerr << "Unable to read from file" << fileName << endl;
        exit(1);
    } else {
        // Safely use the file stream
        ChunkyString message;

        char c;
	while (fileReader.get(c)) {
  	    message.push_back(c);
	}
	
	NoisyTransmission transmissionLine{noiseLevel};
	transmissionLine.transmit(message);

        cout << message << endl;
        return 0;
    }

}
