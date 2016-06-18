#include "Simulator.h"

namespace simulator {

using namespace std;

/**
 * @brief Virtual destructor required for polymorphism. Does nothing.
 */
TNode::~TNode()
{
}

/**
 * @brief Delivers a message to all the suscriptors for that type of message.
 *
 * @param message The message to deliver.  The type is extracted with getType method.
 */
void TNode::putMessage(TMessage *message)
{
    Simulator::getInstance()->notify("node.message." + toLower(message->getType()), message, NULL, this);        
    
    string type = message->getType();
    multimap<string, TMessageReceiver*>::iterator it = messageReceivers.find(type);
    
    while ((it != messageReceivers.end()) && (it->first == type)) {
        Simulator::getInstance()->addEvent(new ReceiveMessageEvent(0, it->second, message), true);
        it++;
    }
}

/**
 * @brief Register a new message listener.  
 * The destination will start receiving messages of that type.
 *
 * @param type Type of message to register for.
 * @param destination the object where "onMessageReceived" will be called to deliver them.
 */
void TNode::registerMessageListener(const string &type, TMessageReceiver *destination)
{
    messageReceivers.insert(pair<string, TMessageReceiver*>(type, destination));
}

/**
 * @brief Unregister a message listener.  
 * If not found, it returns silently.
 *
 * @param type Type of message that the listener wants to unregister for.
 * @param destination the listener to be unregistered.
 */
void TNode::unregisterMessageListener(const string &type, TMessageReceiver *destination)
{
    multimap<string, TMessageReceiver*>::iterator it = messageReceivers.find(type);
    
    while ((it != messageReceivers.end()) && (it->first == type)) {
        if (it->second == destination) {
            messageReceivers.erase(it);
            return;
        }
        it++;
    }

}

}
