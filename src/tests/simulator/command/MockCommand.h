#ifndef _MOCKCOMMAND_H_
#define _MOCKCOMMAND_H_

#include <map>
#include "Command.h"
#include "Notification.h"
#include "MultiCommandRunner.h"
#include "CommandQueryResult.h"


namespace simulator {
    namespace command {
namespace tests {

using namespace std;
using namespace simulator::notification;

/**
 * @brief Mock command runner used for testing.
 */
class MockCommandRunner : public TCommandRunner {
    public:
        MockCommandRunner(string name) : name(name) {};
        
        TCommandResult *runCommand(const Function &t) {
             
            if (t.getName() == "get") 
            {
                return childs[t.getStringParam(0)];
            }

            if (t.getName() == "setNoParam") {
                value = "NoParam";
                return NULL;
            }

            if (t.getName() == "setValue") {
                value = t.getStringParam(0);
                return NULL;
            }
            if (t.getName() == "setInt") {
                value = t.getStringParam(0);
                intValue = t.getIntParam(1);
                return NULL;
            }
            if (t.getName() == "setBool") {
                value = t.getStringParam(0);
                boolValue = t.getBoolParam(1);
                return NULL;
            }

            if (t.getName() == "getMany") {
                vector<TCommandRunner *>runners;
                for (int i = 0; i < t.getParamCount(); i++) {
                    runners.push_back(childs[t.getStringParam(i)]);
                }
                    
                return new MultiCommandRunner(runners);
            }
            
            if (t.getName() == "query") {
                QueryResult *qr = new QueryResult(name);
                qr->insert("value", value);
                return new CommandQueryResult(qr);
            }
            
            return NULL;
        }
        
        void addChild(MockCommandRunner *child) {
            childs[child->getName()] = child;
        }
        
        string getValue() const {
            return value;
        }        
        string getName() const {
            return name;
        }        
        int getIntValue() const {
            return intValue;
        }
        bool getBoolValue() const {
            return boolValue;
        }

    private:
        string value;
        int intValue;
        bool boolValue;
        string name;
        map<string, MockCommandRunner*> childs;
    
};
    
}
}
}        

#endif
