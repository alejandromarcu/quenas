#ifndef _COMMANDQUERYRESULT_H_
#define _COMMANDQUERYRESULT_H_

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
 * @brief  An object of this class is returned by a command runner to 
 * encapsulate a Query Result.
 */
class CommandQueryResult : public TCommandResult {    
    public:
        /**
         * @brief Create a CommandQueryResult with the specified QueryResult.
         *
         * @param result result of a query.
         */
        CommandQueryResult(QueryResult *result) : result(result) {};
        
        /**
         * @brief Get the QueryResult set in the constructor.
         *
         * @return the QueryResult set in the constructor.
         */        
        QueryResult *getQueryResult() { return  result; }        
    private:
        
        /// The QueryResult set in the constructor.        
        QueryResult *result;
};
        
        
}
}


#endif
