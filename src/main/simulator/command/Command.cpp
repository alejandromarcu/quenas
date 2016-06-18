#include <string>
#include <iostream>
#include "Command.h"
#include "common.h"
#include "Notification.h"
#include "Simulator.h"
#include "Exceptions.h"
#include "MultiCommandRunner.h"
#include "CommandQueryResult.h"

namespace simulator {
    namespace command {

using namespace std;
using namespace simulator;
using namespace simulator::notification;



/**
 * @brief Create a command object from a command string.
 * The string is parsed in functions and they are stored.
 *
 * @param cmd the command string.
 */
Command::Command(const string &cmd)
{
    try {
        // a command is composed by several functions separated by a dot.
        vector<string> f = split(cmd, '.');
    
        for (int i=0; i < f.size(); i++) {
            functions.push_back(Function(f[i]));
        }
    } catch (invalid_argument &ia) {
        throw command_error("Can't parse the command '" + cmd + "' due to: " + ia.what());
    }   
}

/**
 * @brief Run a command in a destination object.
 *
 * @param destination object where the command is run.
 * @return the result of the command encapsulated into a TCommandResult.
 */
TCommandResult *Command::run(TCommandRunner *destination)
{
    // just use the recursive method starting from 0.
    return run(destination, 0); //valgrind: agregue el return
}

/**
 * @brief Run a command in a destination object.
 *
 * This method recursively calls himself when needed using as the destination
 * the object encapsulated in the command result.
 *
 * @param destination object where the command is run.
 * @param start the function to start running the command.
 * @return the result of the command encapsulated into a TCommandResult.
 */
TCommandResult *Command::run(TCommandRunner *destination, int start)
{
    int n = functions.size();

    if (destination == NULL) {
        throw command_error("Null destination for command " + start);
    }
  
    // if it is the last function, just return the result of executing the function
    // without further processing.  
    if (start == (n - 1)) {
        return destination->runCommand(functions[n - 1]);
    }

    // run the command in the destination!
    TCommandResult *result = destination->runCommand(functions[start]);

    // only the last function could return null
    if (result == NULL) {
        throw command_error("Null received when executing " + functions[start].toString());
    }

    // if the result is a command runner, then the next function is executed in
    // that command runner.
    TCommandRunner *runner = dynamic_cast<TCommandRunner*>(result);
    if (runner != NULL) {
        return run(runner, start+1);
    }
    
    // if the result is a MultiCommandRunner, then each of the runners contained
    // executes the next function, and the results are aggregated.
    MultiCommandRunner *multi = dynamic_cast<MultiCommandRunner*>(result);
    if (multi != NULL) {
        vector<TCommandRunner *> runners =  multi->getRunners();
        
        QueryResult *qr = new QueryResult(destination->getName(), destination->getId());
        
        for (int i = 0; i < runners.size(); i++) 
        {
            TCommandResult *cr = run(runners[i], start + 1);
        
            // if the result is a QueryResult, aggregate it.
            CommandQueryResult *cqr = dynamic_cast<CommandQueryResult *>(cr);
            if (cqr != NULL) {
                if (cqr->getQueryResult()->getName() == runners[i]->getName()) {
                    qr->insert(cqr->getQueryResult()->getName(), cqr->getQueryResult());
                } else {                    
                    QueryResult *inside = new QueryResult(runners[i]->getName(),runners[i]->getId());
                    inside->insert("",cqr->getQueryResult());
                    qr->insert("", inside);
                }
                
                delete cr; // valgrind
            }

        }
        
        delete result; // valgrind
                
        // if the result report is empty return null, if not return it encapsulated.
        return qr->getProperties().size() > 0? new CommandQueryResult(qr) : NULL;    
    } 
    
    // the result can't be a CommandQueryResult if it is not the last function.
    CommandQueryResult *cqr = dynamic_cast<CommandQueryResult *>(result);
    if (cqr != NULL) {
        throw command_error("Unexpected CommandQueryResult received when executing " + functions[start].toString());
    } 
}


}}
