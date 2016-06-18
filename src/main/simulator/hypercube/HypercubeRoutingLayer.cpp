#include <vector>
#include <iterator>

#include "common.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "StateMachines.h"
#include "ReactiveRouting.h"
#include "HypercubeNode.h"
#include "RouteHeader.h"
#include "UDPSegment.h"
#include "HypercubeMaskAddress.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::hypercube::routing;
using namespace simulator::hypercube;
using namespace simulator::message;
using namespace simulator::event;

/**
 * @brief Create an Hypercube Routing Layer.
 *
 * @param dll Data Link Layer below
 * @param hcl HypercubeControlLayer of the node.
 */
HypercubeRoutingLayer::HypercubeRoutingLayer(TDataLinkLayer *dll, HypercubeControlLayer *hcl) 
    : TNetworkLayer(dll->getNode(), dll)
{
    routing = new ReactiveRouting(dynamic_cast<HypercubeNode *>(dll->getNode()));
    this->hcl = hcl;
}

/**
 * @brief Send a segment to a node.
 * This method is intendeed to be called from the transport protocol layer above.
 *
 * @param dest destination address.
 * @param protocol transport protocol of the above layer.
 * @param segment segment to send.
 */
void HypercubeRoutingLayer::send(const TNetworkAddress &dest, const TTransportProtocolId &protocol, const TSegment &segment)
{
    HypercubeAddress hcDest = dynamic_cast<const HypercubeAddress &>(dest);
    HypercubeNode *hcNode = dynamic_cast<HypercubeNode*>(getNode());
 
    if (!hcNode->isConnected()) return;
   
    DataPacket dp(hcNode->getPrimaryAddress(), hcDest, 
                  dynamic_cast<const TransportType &>(protocol),segment);

    Simulator::getInstance()->notify("node.sent.hcpacket.data", &dp, NULL, getNode());

    // Check if the packet is the special case of TraceRoute, RendezVousServer or Client
    const UDPSegment *udp = dynamic_cast<const UDPSegment *>(&segment);
    if (udp != NULL) {
         if (udp->getSourcePort() == TraceRoute::PORT) dp.setTraceRoute(true);
         if (udp->getDestinationPort() == RendezVousServer::PORT) dp.setRendezVous(true);
    }

    // If the destination is the same as the source, send it directly 
    // to transport layer (no actual routing needed).
    if (hasArrived(dp)) {
        TTransportLayer *tl = transportProtocols[dp.getTransportType()];
        if (tl == NULL) throw invalid_argument("Unknown transport type: " + toStr((int) dp.getTransportType().getType()));
        
        tl->receive(dp.getSourceAddress(), dp);
        return;
    }
    
    HypercubeAddress nextHop = routing->route(&dp);    
    sendToRoute(nextHop, &dp);
}

/**
 * @brief Return whether the final destination of the packet is this node.
 * If the packet is rendez vous, is not only the node but its managed space.
 *
 * @param dp packet to check.
 * @return whether the final destination of the packet is this node.
 */
bool HypercubeRoutingLayer::hasArrived(const DataPacket &dp)  
{    
    // accept any address of the managed space for Rendez Vous
    if (dp.isRendezVous()) {
        vector<HypercubeMaskAddress>::const_iterator it;
        for (it = hcl->getAddresses().begin(); it != hcl->getAddresses().end(); it++) {
            if (it->contains(dp.getDestinationAddress())) return true;
        }
        return false;

    } else {
        HypercubeMaskAddress nodeAddr = hcl->getPrimaryAddress();
   
        // exactly equal if not Rendez Vous
        return nodeAddr == dp.getDestinationAddress();
    }
}
    
/**
 * @brief Receive a frame.
 * If the frame has arrived to the final destination, send to the upper layer,
 * if it has not, route it.
 *
 * @param from address of the neighbour that sent the frame.
 * @param frame frame received.
 */    
void HypercubeRoutingLayer::receive(const TPhysicalAddress &from, const TFrame &frame)
{
    HypercubeNode *hcNode = dynamic_cast<HypercubeNode*>(getNode());
    if (!hcNode->isConnected()) return;

    DataPacket dp;
    dp.read(frame);


    if (dp.getTTL() == 0) {
       Simulator::getInstance()->notify("packet.discarded", &dp, NULL, getNode());
       return;
    }

    Simulator::getInstance()->notify("node.received.hcpacket.data", &dp, NULL, getNode());

    map<MACAddress, Neighbour>::iterator it = hcl->getNeighbours().find(dynamic_cast<const MACAddress&>(from));
    if (it == hcl->getNeighbours().end()) {
      throw invalid_argument("Physical address not in routing table:" + from.toString() + " in node " + hcNode->getId());
    }

    
    
    if (hasArrived(dp)) {
        TTransportLayer *tl = transportProtocols[dp.getTransportType()];
        if (tl == NULL) throw invalid_argument("Unknown transport type: " + toStr((int) dp.getTransportType().getType()));

        if (dp.isTraceRoute()) {
            notifyTraceRoute(dp);
        }
        
        tl->receive(dp.getSourceAddress(), dp);
    } else {    
        HypercubeAddress nextHop = routing->route(&dp, it->second.getPrimaryAddress());            
        
        sendToRoute(nextHop, &dp);              
    }
}

/**
 * @brief Helper method to notify that a trace route has been completed.
 *
 * @param dp packet containing the trace route.
 * @param hasRoute whether a route could be found or not.
 */
void HypercubeRoutingLayer::notifyTraceRoute(const DataPacket &dp, bool hasRoute)
{
    string data = Data(UDPSegment(dp).getPayload()).toString();

    string got = "(";
    QueryResult *qr = new QueryResult("TraceRoute", data);
    
    vector<RouteHeader> route = dp.getRoute();

    if (hasRoute) qr->insert("distance", toStr(route.size()));
    else qr->insert("noRoute", "true");
    
    // add each hop
    for (int i = 0; i < route.size() ; i++) {
        HypercubeNetwork *net = dynamic_cast<HypercubeNetwork*>(Simulator::getInstance()->getNetwork());
        HypercubeNode *node = dynamic_cast<HypercubeNode *>(net->getNode(route[i].getAddress()));
        
        QueryResult *hop = new QueryResult("hop");
        hop->insert("node", route[i].getAddress().toString());        
        hop->insert("nodeName", node->getUniversalAddress().toString());
        
        if (route[i].isReturned()) hop->insert("returned", "true");        
        qr->insert("hop", hop);        
        got = got + node->getUniversalAddress().toString() + ";";
    }

    Simulator::getInstance()->notify("node.routing.trace", qr);        

    // if it was an assert, check that the route was the expected.
    if (data.substr(0,6) == "Assert") {
        int pos = data.find("=(");
        string expected = data.substr(pos + 1, data.size() - pos);
        got = got.substr(0, got.length() - 1) + ")";
        
        if (!hasRoute) got = "()";
        if (expected != got) throw invalid_argument("route assert failed. Expected " + expected + ", got " + got);        
    }
}

/**
 * @brief Send a packet to the next hop.
 * It needs to lookup the physical address of the next hop.
 *
 * @param nextHop address of the next hop for the routing.
 * @param dp packet to send.
 */
void HypercubeRoutingLayer::sendToRoute(const HypercubeAddress &nextHop, DataPacket *dp)
{
    // If nextHop is empty, it's because there is no route to host.
    if (nextHop.getBitLength() == 0) {
        if (dp->isTraceRoute()) {
            notifyTraceRoute(*dp, false);
        }

        QueryResult *qr = new QueryResult("route");
        qr->insert("source", dp->getSourceAddress().toString());
        qr->insert("destination", dp->getDestinationAddress().toString());        
        
        Simulator::getInstance()->notify("node.routing.no_route", qr);                
        return;
    }

    // If the packet is flaged to trace route, add the route to the header
    if (dp->isTraceRoute()) {
        dp->addRouteHeader(RouteHeader(nextHop, dp->isReturned()));
    }
    
    // lookup the physical address of the next hop and when found, send to it.
    map<MACAddress, Neighbour>::iterator it= hcl->getNeighbours().begin();
    for (; it != hcl->getNeighbours().end() ; it++) {
        Neighbour n = it->second;
        if (it->second.getPrimaryAddress() == nextHop) {
            getDataLinkLayer()->send(it->second.getPhysicalAddress(), DataPacket::ETHERNET_TYPE, *dp);            
            return;
        }
    }
    
    // couldn't find a physical address for the next hop.
    throw invalid_argument("routing returned an unkown address: " + nextHop.toString());
}


/**
 * @brief Register or unregister a transport protocol in this layer.
 *
 * @param id id of the transport protocol to register
 * @param transportLayer pointer to the transport layer class to register, or NULL to unregister it.
 */
void HypercubeRoutingLayer::registerTransportProtocol(const TTransportProtocolId &id, TTransportLayer *transportLayer)
{
    const TransportType &ipp = dynamic_cast<const TransportType &>(id);
    if (transportLayer == NULL)  {
        transportProtocols.erase(transportProtocols.find(ipp));
    } else {
        transportProtocols[ipp] = transportLayer;
    }
    
}

/**
 * @biref get the Routing algorithm class.
 *
 * @return the Routing algorithm class.
 */
TRoutingAlgorithm *HypercubeRoutingLayer::getRouting() 
{
    return routing; 
}

}
}

