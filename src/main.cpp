#include <cstdlib>
#include <iostream>
#include <ctime>

#include "HypercubeAddress.h"
#include "common.h"
#include <iterator>
#include <stdexcept>
#include "TConnection.h"
#include "PhysicalLayer.h"
#include "Event.h"
#include "Units.h"
#include "Simulator.h"
#include "MACAddress.h"
#include "Connection.h"
#include "MockLayer.h"
#include "Message.h"
#include "StateMachines.h"


using namespace std;
using namespace simulator;
using namespace simulator::address;

namespace simulator {
    void runAllTests();
}


/**
 * @brief Run a simulation, reading a simulation input file and writing the output to another file.
 *
 * @param inFile name of the input file.
 * @param outFile name of the output file.
 */
void runSimulation(char *inFile, char *outFile)
{
    Simulator *sim = Simulator::getInstance();
    sim->setShowProgress();
    clock_t startTime;
    try {
        sim->getNotificator().setFilename(outFile);

        cout << "Loading file " << inFile << "..." << endl;
        sim->loadFile(inFile);

        cout << "Running simulation. Output being sent to " << outFile << "..." << endl;
        startTime = clock();

        sim->simulate();
    } catch(exception &e) {
        cout << "ERROR: " <<  e.what() << endl;
    }
    clock_t endTime = clock();

#ifdef CLK_TCK
    cout << endl << "Done in " << (((double) endTime-startTime) / CLK_TCK) << " seconds." << endl;
#endif    
    cout << "Simulated time: " << sim->getTime().toString(Time::SEC) << endl;
    Simulator::destroy();

}

/**
 * @brief Main method for the simulator.
 *
 * Run with "-test" to run unit tests, or with "input output" to read from input file and write to output.
 *
 * @param argc number of arguments.
 * @param argv arguments passed from command line.
 */
int main(int argc, char *argv[])
{
   if (argc == 2 && string(argv[1]) == "-test") {
       runAllTests();
       return EXIT_SUCCESS;
   }

   if (argc != 3) {
        cout << "Usage:" << endl;
        cout << "    quenas input output" << endl << endl;
        cout << " For running internal tests:" << endl;
        cout << "    quenas -test" << endl << endl;
        return EXIT_SUCCESS;
   }


 	runSimulation(argv[1], argv[2]);
 	return EXIT_SUCCESS;
}
