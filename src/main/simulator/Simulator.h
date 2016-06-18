#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <queue>
#include <map>

#include "Units.h"
#include "common.h"
#include "Event.h"
#include "Notification.h"
#include "Command.h"
#include "UniversalAddress.h"
#include "PhysicalLayer.h"
#include "DataLinkLayer.h"
#include "Message.h"
#include "TypeFilter.h"
#include "Applications.h"
#include "TNetwork.h"
#include "TNode.h"

namespace simulator {
    
using namespace simulator::event;    
using namespace simulator::notification;    
using namespace simulator::command;    
using namespace simulator::address;    
using namespace simulator::layer;  
using namespace simulator::message;
using namespace simulator::hypercube;

using namespace std;
    
/**
 * @brief Main class for the simulator.
 * It is a singleton, so that any class can access it through Simulator::getInstance()
 */
class Simulator : public TCommandRunner {
    public:
        static Simulator *getInstance();
        static void destroy();
        
        Time getTime() const;

        bool simulateStep(Time maxTime = -1);
        void simulate();        

        void loadFile(const string &fileName);
        
        void addEvent(TEvent *event, bool timeRelative=false);
        void reset();
        
        void notify(const string &notificationType, TNode *node);
        void notify(const string &notificationType, QueryResult *queryResult, const string &extraDataName, const string &extraDataValue);
        void notify(const string &notificationType, QueryResult *queryResult, TNode *node = NULL);
        void notify(const string &notificationType, const TQueryable *object, vector<string> *queryOptions = NULL, TNode *node = NULL);

        void writeNotification(const string &notificationType, QueryResult *queryResult, const string &extraDataName="", const string &extraDataValue="");

        void exec(TCommandRunner *dest, const string &cmd);
        void exec(const string &cmd);
        
        void setNotifFilter(TNotifFilter *notifFilter);
        TNotifFilter *getNotifFilter() const;
        
        Notificator &getNotificator();
        
        void setNetwork(TNetwork *network);
        TNetwork *getNetwork() const;
        
        void setEndTime(Time t);
        
        void setShowProgress(bool show=true); // newVersion

        virtual TCommandResult *runCommand(const Function &f);
        virtual string getName() const;
       
    private:
        Simulator();
        
        /// Current time of the simulation
        Time time;
        
        /// Time when the simulation should end
        Time endTime;
        
        //
        int percent;
        
        /// Priority queue for the events to be executed
        priority_queue<TEvent *> eventQueue;   

        /// Object used to write notifications
        Notificator notificator;        

        /// Object used to filter notifications
        TNotifFilter *notifFilter;
        
        /// The network that is being simulated.
        TNetwork *network;

        /// Whether to show the progress of the simulation
        bool showProgress; // newVersion
        
        /// Unique instance of the simulator
        static Simulator *instance;
};

}

#endif
