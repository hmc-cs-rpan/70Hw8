/**
 * \file message-passer.cpp
 * \author Fill me in!
 *
 *
 * \remarks
 *
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
 * \param seed          Optional seed value for random number generator
 * \param filename      Name of file to read original message from
 * \param noiseLevel    Likelihood of a character being modified
 */
void processOptions(list<string> options,
		    string& filename,
                    float& noiseLevel)
{

    // Loop through the options. If they start with a dash
    // try to interpret them as a flag; if not, assume it's
    // the number of insertions.
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
	    noiseLevel = stod(value);
        } else if (flag == "-f" || flag == "--filename") {
            filename = value;
        } else {
            cerr << "Unrecognized option: " << flag << endl;
            cerr << "Usage: ./messagePasser -n noise -f filename" << endl;
            exit(2);
        }
    }

    return;
}



int main(int argc, const char* argv[])
{
    float noiseLevel = 0;
    string fileName;

    list<string> options(argv + 1, argv + argc);
    processOptions(options, fileName, noiseLevel);

    ifstream fileReader( fileName );

    if (!fileReader.is_open()) {
        // The file could not be opened
        cerr << "Unable to read from file" << fileName << endl;
        exit(-1);
    } else {
        // Safely use the file stream
        string messageLine; 
        ChunkyString message;

	while (getline(fileReader, messageLine)) {
  	    message += messageLine + "\n";
	}

	NoisyTransmission transmissionLine{noiseLevel};
	transmissionLine.transmit(message);

        cout << message << endl;
        return 0;
    }

}
