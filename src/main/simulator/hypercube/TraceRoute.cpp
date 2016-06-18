#include <vector>
#include <iterator>


#include "TraceRoute.h"
#include "common.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "StateMachines.h"
#include "HypercubeNode.h"
#include "Exceptions.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;
      
const int TraceRoute::PORT = 9901;
      
/**
 * @brief Create a TraceRoute Application and bind it to the transport layer.
 *
 * @param tl pointer to the Transport Layer below this application.
 */        
TraceRoute::TraceRoute(TTransportLayer *tl) : HypercubeBaseApplication(tl)
{
    bind(PORT);
}

/**
 * @brief Unbind the application from the port.
 */
TraceRoute::~TraceRoute()
{
    unbind();
}

/**
 * @brief Run a Command.
 *
 * @param function the function to run.
 * @return a pointer to the next command result to execute.
 */        
TCommandResult *TraceRoute::runCommand(const Function &function)
{
    if (function.getName() == "trace")  
    {       
        QueryResult *qr = new QueryResult(getName(), getId());
        HypercubeAddress addr(function.getStringParam(0));
        Data data("Trace Route at " + Simulator::getInstance()->getTime().toString(Time::SEC) + 
            " from " + dynamic_cast<HypercubeNode *>(getNode())-> getPrimaryAddress().toString() +
             " to " + addr.toString());
        transportLayer->send(addr, Port(PORT), Port(PORT), data);
        return this;
    }

    if (function.getName() == "assert")  
    {       
        QueryResult *qr = new QueryResult(getName(), getId());
        HypercubeAddress addr(function.getStringParam(0));
        Data data("Assert Route at " + Simulator::getInstance()->getTime().toString(Time::SEC) + 
            " from " + dynamic_cast<HypercubeNode *>(getNode())-> getPrimaryAddress().toString() +
             " to " + addr.toString() +
             "=(" + function.getStringParam(1) + ")");
        transportLayer->send(addr, Port(PORT), Port(PORT), data);
        return this;
    }

    if (function.getName() == "traceUAddr")  
    {       
        QueryResult *qr = new QueryResult(getName(), getId());
        string uaddr = function.getStringParam(0);
        Data data("Trace Route at " + Simulator::getInstance()->getTime().toString(Time::SEC) + 
            " from " + dynamic_cast<HypercubeNode *>(getNode())->getUniversalAddress().toString() +
             " to " + uaddr);
        send(uaddr, Port(PORT), data);
        return this;
    }
    
    throw command_error("TraceRoute: Invalid function: " + function.getName());    
}

/**
 * @brief Get the object name
 *
 * @return "TraceRoute"
 */
string TraceRoute::getName() const
{
    return "TraceRoute";
}

/**
 * @brief Receive data.  Nothing needs to be done since receiving a trace route
 * is handled in the routing layer.
 *
 * @param from address of the sending node
 * @param sourceAppId the Application Id (port) of the sender
 * @param data data received
 */
void TraceRoute::receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet)
{
}    
    




}
}

