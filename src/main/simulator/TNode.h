#ifndef _TNODE_H_
#define _TNODE_H_

#include <queue>
#include <map>

/*
#include "Units.h"
#include "common.h"
#include "Event.h"
#include "Notification.h"
#include "Command.h"
#include "UniversalAddress.h"
#include "PhysicalLayer.h"
#include "DataLinkLayer.h"
#include "TypeFilter.h"
#include "Applications.h"
*/
#include "Message.h"

namespace simulator {

/**
 * @brief Base class for nodes in the network.
 *
 * It provides functionallity for register and send messages inside the node.
 */
class TNode : public TCommandRunner {
    public:
        virtual ~TNode();
        void putMessage (TMessage *message);
        void registerMessageListener(const string &type, TMessageReceiver *destination);
        void unregisterMessageListener(const string &type, TMessageReceiver *destination);        

    private:
        /// Stores for each type of message, who are registered to receive them.
        multimap<string, TMessageReceiver*> messageReceivers;
    
};

}

#endif
