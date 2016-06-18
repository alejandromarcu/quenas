#ifndef _MULTICOMMANDRUNNER_H_
#define _MULTICOMMANDRUNNER_H_

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
 * @brief This is a command result containing a collection of command runners.
 *
 * An instance of this class is returned by a command runner when the command
 * wants to return a collection of command runners.  For example, when the
 * network is asked for all its nodes, it can return a collection of its nodes 
 * encapsulated in this object.
 */
class MultiCommandRunner: public TCommandResult {
    public:
        /**
         * @brief Create a multi command runner storing the specified command runners.
         * 
         * @param runners command runners to store.
         */
        MultiCommandRunner(vector<TCommandRunner *> runners) : runners(runners) {};
        
        /**
         * @brief Get the runners collection set in the constructor.
         *
         * @return the runners collection set in the constructor.
         */
        vector<TCommandRunner *> getRunners() const { return runners; } 
    private:
        
        /// Contains the runners collection set in the constructor.
        vector<TCommandRunner *> runners;
};

        
        
}
}


#endif
