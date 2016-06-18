#include <iostream>
#include <cmath>

#include "TPhysicalAddress.h"
#include "PhysicalLayer.h"
#include "MACAddress.h"
#include "BitStream.h"
#include "Connection.h"
#include "Units.h"
#include "Simulator.h"

namespace simulator {
    namespace layer {

using namespace std;
using namespace simulator::dataUnit;

/**
 * @brief Create a Physical Layer.
 *
 * It just stores the node and address.
 *
 * @param node pointer to the node containing this layer.
 * @param address addess associated to this layer.
 */
PhysicalLayer::PhysicalLayer(TNode *node, const MACAddress &address) : TPhysicalLayer(node), address(address)
{
}

/**
 * @brief Destructor for the physical layer.
 * It erases all the connections to this Physical Layer.
 */
PhysicalLayer::~PhysicalLayer()
{
    vector<TConnection *> conns;                               
    for (map<MACAddress, TConnection*>::iterator it = connections.begin(); it != connections.end(); it++) {
        conns.push_back(it->second);
    }
    
    for (vector<TConnection *>::iterator it = conns.begin(); it != conns.end(); it++) {
        delete *it;
    }

}


/**
 * @brief Send a frame to another physical layer that is connected to this one.
 *
 * It calculates when is the right time to send the frame to simulate the queue
 * due to finite bandwidth, and schedules a SendBitStreamEvent for that time.
 *
 * @param dest physical address destination for the frame.
 * @param frame the frame to send.
 */
void PhysicalLayer::send(const TPhysicalAddress &dest, const TFrame &frame)
{
    // Adjust nextTimeToSend to be at least current time    
    if (nextTimeToSend.getValue() < Simulator::getInstance()->getTime().getValue()) {
        nextTimeToSend = Simulator::getInstance()->getTime();
    }

    // dest must be always of type MACAddress
    MACAddress mac = dynamic_cast<const MACAddress &>(dest);

    // If broadcast, send using a different method.
    if (mac == MACAddress::BROADCAST) {
        sendBroadcast(frame);
        return;
    }
    
    // Get the connection to that mac address
    TConnection *conn = connections[mac];
    
    if (conn == NULL) {
        throw invalid_argument("Connection not found");
    }
    
    // build a BitStream that encapsulates this frame
    BitStream bs(frame);
    
    // zero or negative bandwidth indicate infinite bandwidth
    if (conn->getBandwidth().bpsValue() > 0) {
        nextTimeToSend += (long long) round(((double) bs.getLength() / conn->getBandwidth().bpsValue()) * 8.0 * Time::SEC); 
    }
    
    TEvent *send = new SendBitStreamEvent(nextTimeToSend, this, conn, bs);
    Simulator::getInstance()->addEvent(send);
    

}

/**
 * @brief Send a frame as Broadcast, i.e. to all connected nodes.
 *
 * It calculates when is the right time to send the frame (taking into account
 * the connection that has minimum bandwidth) to simulate the queue
 * due to finite bandwidth, and schedules SendBitStreamEvent for that time.
 *
 * @param frame the frame to send.
 */
void PhysicalLayer::sendBroadcast(const TFrame &frame)
{
    map<MACAddress, TConnection*>::iterator it;
    
    long minBandwidth = 0;
    
    // search minimum bandwidth
    for (it = connections.begin(); it!= connections.end(); it++) {            
        long bw = it->second->getBandwidth().bpsValue();
        if ((minBandwidth == 0) || ((bw > 0)) &&  (minBandwidth > bw)) {
            minBandwidth = bw;
        }
    }

    // build a BitStream that encapsulates this frame
    BitStream bs(frame);
    
    // zero or negative bandwidth indicate infinite bandwidth
    if (minBandwidth > 0) {
        nextTimeToSend += (long long) round(((double) bs.getLength() / minBandwidth)  * 8.0 * Time::SEC); // bandwidth: bytes/sec
    }
    
    // send through all the connections.
    for (it = connections.begin(); it!= connections.end(); it++) {            
        TEvent *send = new SendBitStreamEvent(nextTimeToSend, this, it->second, bs);
        Simulator::getInstance()->addEvent(send);
    }
    
}


/**
 * @brief Return the address associated with this layer.
 * 
 * @return the address associated with this layer.
 */
const MACAddress &PhysicalLayer::getAddress() const
{
    return address;
}

/**
 * @brief This method must be called when a bit stream was received.
 *
 * @param bitStream the received bit stream.
 */
void PhysicalLayer::receive(const TBitStream &bitStream)
{
    dataLinkLayer->receive(bitStream);
}
        

/**
 * @brief Set a pointer to the Data Link Layer that is associated with this physical layer.
 *
 * @param dataLinkLayer pointer to the Data Link Layer that is associated with this physical layer.
 */
void PhysicalLayer::setDataLinkLayer(TDataLinkLayer *dataLinkLayer) 
{
    this->dataLinkLayer = dataLinkLayer;
}
        

/**
 * @brief Add a connection to this physical layer.
 *
 * @param conn a pointer to the connection to be added.
 */
void PhysicalLayer::addConnection(TConnection *conn)
{
    const vector<TPhysicalLayer *> points = conn->getPoints();
        
    // For each point in the connection different than this one, add it in
    // the connection map using the MACAddress as the key and the 
    // connection as the value
    for (int i = 0; i < points.size(); i++) {
        MACAddress mac = dynamic_cast<const MACAddress&>(points[i]->getAddress()); 
    
        if (mac != address) {
            connections[mac] = conn;
        }
    }
}

/**
 * @brief Remove an existing connection to this physical layer.
 *
 * @param conn a pointer to the connection to be removed.
 */
void PhysicalLayer::removeConnection(TConnection *conn)
{
    const vector<TPhysicalLayer *> points = conn->getPoints();
    
    // For each point in the connection different than this one, 
    // remove it from the connections map
    for (int i = 0; i < points.size(); i++) {
        MACAddress mac = dynamic_cast<const MACAddress&>(points[i]->getAddress()); 
    
        if (mac != address) {
            map<MACAddress, TConnection*>::iterator it = connections.find(mac);
            
            connections.erase(it);
        }
    }
    
}

/**
 * @brief Get a map of connections for this physical layer.
 *
 * @return a map of connections for this physical layer.
 */
map<MACAddress, TConnection*> &PhysicalLayer::getConnections()
{
    return connections;
}

}
}
