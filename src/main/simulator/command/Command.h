#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>
#include "Units.h"
#include "Notification.h"
#include "TCommandResult.h"

namespace simulator {
    namespace command {

using namespace simulator;
using namespace std;
using namespace simulator::notification;


/**
 * @brief Stores and executes a command.
 *
 * A command is composed by one or more functions, for example: "allNodes.setDelay(1)".
 * The command is built from a string and parsed into functions.
 */
class Command {
    public:
        Command(const string &cmd);
        TCommandResult *run(TCommandRunner *destination);
    private:
        TCommandResult *run(TCommandRunner *destination, int start);

        /// Functions that compose this command
        vector<Function> functions;
        
        /// The string used to create this command.
        string command;            
};

       
}
}


#endif
