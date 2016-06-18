#include <iostream>
#include <cmath>

#include "TPhysicalAddress.h"
#include "PhysicalLayer.h"
#include "BitStream.h"
#include "Connection.h"
#include "Units.h"
#include "Simulator.h"
#include "Exceptions.h"
#include "MultiCommandRunner.h"
#include "CommandQueryResult.h"
#include "HypercubeNetwork.h"

namespace simulator {
    namespace layer {

using namespace std;
using namespace simulator::dataUnit;

/** 
 * @brief Creates a connection between two endpoints
 *
 * @param point1 one of the endpoints of the connection.
 * @param point2 the other endpoint of the connection.         
 * @param bandwidth the bandwith in bytes/sec this connection can carry.
 * @param delay the delay in seconds from that connection.
 */
Connection::Connection(TPhysicalLayer *point1, TPhysicalLayer *point2, Bandwidth bandwidth, Time delay) :
    point1(point1), point2(point2), bandwidth(bandwidth), delay(delay) 
{
    point1->addConnection(this);
    point2->addConnection(this);      
}

/**
 * @brief Clears this connection.
 * 
 * It removes itself from the two endpoints.
 */
Connection::~Connection()
{
    point1->removeConnection(this);
    point2->removeConnection(this);        
}

/**
 * @brief Transports a bitStream to the endpoint that is not "from"
 *
 * @param from the node that is sending the bit stream.
 * @param bitStream bit stream to be transported.
 */
void Connection::transport(TPhysicalLayer *from, const TBitStream &bitStream) 
{
    TPhysicalLayer *dest;
    
    // determine which endpoint will receive the bit stream.
    if (from == point1) dest = point2;
    else if (from == point2) dest = point1;
    else throw invalid_argument("the connection is not connected to the from node");
    
    // schedule an event so that the endpoint receives the bit stream after the delay.
    TEvent *send = new ReceiveBitStreamEvent(delay, dest, dynamic_cast<const BitStream&>(bitStream));
    Simulator::getInstance()->addEvent(send, true);
}

/**
 * @brief Get the two endpoints of this connection.
 *
 * @return a vector of size 2 wuith the two endpoints
 */
vector<TPhysicalLayer *> Connection::getPoints() const 
{
    vector<TPhysicalLayer *> points;
    points.push_back(point1);
    points.push_back(point2);            
    return points;
};

/**
 * @brief Get the bandwidth for this connection.
 *
 * @return the bandwidth for this connection.
 */
Bandwidth Connection::getBandwidth() const
{
    return bandwidth;
}

/**
 * @brief Get the delay for this connection.
 *
 * @return the delay for this connection.
 */
Time Connection::getDelay() const
{
    return delay;
}
     
/**
 * @brief Run a command.
 *
 * @param f function to run.
 */      
TCommandResult *Connection::runCommand(const Function &f) {
    if (f.getName() == "query") 
    {
        QueryResult *qr = new QueryResult(getName(), getId());
        qr->insert("endpoint", point1->getAddress().toString());
        qr->insert("endpoint", point2->getAddress().toString());
        qr->insert("bandwidth", bandwidth.toString());                
        qr->insert("delay", delay.toString());                                
        return new CommandQueryResult(qr);
    }            

    if (f.getName() == "setBandwidth")  
    {
        bandwidth = f.getIntParam(0);
        return this;
    }

    if (f.getName() == "setDelay")  
    {
        delay = f.getTimeParam(0);
        return this;
    }
       
    throw command_error("Bad function: " + f.toString());
}
     
/**
 * @brief Get the name of this type of object used for the queries.
 * 
 * @return "Connection"
 */    
string Connection::getName() const 
{
    return "Connection";
}

/**
 * @brief Get an id for this connection.
 * The id is composed by the id's of the nodes in the endpoint with format (id1,id2).
 * 
 * @return an id for this connection with format (id1,id2).
 */         
string Connection::getId() const
{
    return "(" + point1->getNode()->getId() + "," + point2->getNode()->getId() + ")";
}        

/**
 * @brief Set the connection bandwith.
 *
 * @param bw bandwidth for the connection.
 */
void Connection::setBandwidth(Bandwidth bw) 
{ 
    bandwidth = bw; 
}

/**
 * @brief Set the delay for the connection.
 *
 * @param d delay for the connection.
 */
void Connection::setDelay(Time d)
{ 
    delay = d; 
}        

}
}
