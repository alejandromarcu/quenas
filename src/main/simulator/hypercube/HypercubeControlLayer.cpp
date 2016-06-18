#include <vector>
#include <iterator>

#include "common.h"
#include "HCPacket.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "StateMachines.h"
#include "ReactiveRouting.h"
#include "HypercubeNode.h"
#include "RouteHeader.h"
#include "UDPSegment.h"
#include "HypercubeMaskAddress.h"
#include "HypercubeControlLayer.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::hypercube::routing;
using namespace simulator::hypercube;
using namespace simulator::message;
using namespace simulator::event;
      
/**
 * @brief Create a new Hypercube Control layer.
 *
 * @param node pointer to the node containing this layer.
 * @param dll pointer to the data link layer used by this layer.
 */
HypercubeControlLayer::HypercubeControlLayer(TNode *node, TDataLinkLayer *dll) 
    : TNetworkLayer(node, dll), recoveredMask(0), hbEnabled(true)
{
    getNode()->registerMessageListener(ConnectedMessage::TYPE, this);
    getNode()->registerMessageListener(DisconnectedMessage::TYPE, this);
    
    mainSM = new MainSM(this);
    papSM = NULL;
    hblSM = NULL;
    clearPacketStats();
}

/**
 * @brief Get the mask of recovered space.
 *
 * @return the mask of recovered space.
 */        
HypercubeAddress HypercubeControlLayer::getRecoveredMask() const
{
    return recoveredMask;
}

/**
 * @brief Set the mask of recovered space.
 *
 * @param rm the mask of recovered space.
 */        
void HypercubeControlLayer::setRecoveredMask(const HypercubeAddress &rm)
{
    recoveredMask = rm;
}
       
/**
 * @brief This method must not be called because there are no layers above.
 * If called, it throws an exception.
 *
 * @param dest unused
 * @param protocol unused
 * @param segment unused
 */         
void HypercubeControlLayer::send(const TNetworkAddress &dest, const TTransportProtocolId &protocol, const TSegment &segment)
{
    throw new logic_error("HypercubeControlLayer::send must not be called!");
}

/**
 * @brief This method is called when a frame is received.
 * It sends the packet to the state machines.
 * 
 * @param from the physical address from which this segment was sent.
 * @param frame the received frame.
 */
void HypercubeControlLayer::receive(const TPhysicalAddress &from, const TFrame &frame)
{
    TControlPacket *packet = TControlPacket::create(frame);
    
    Simulator::getInstance()->notify("node.received.hcpacket." + toLower(packet->getName()), packet, NULL, getNode());
    
    packetStats[PACKETS_RECEIVED][packet->getType() - 1]++;
    packetStats[BYTES_RECEIVED][packet->getType() -1 ] += packet->getTotalLength();

    mainSM->onPacketReceived(*packet);

    // Heard Bit Listener and Primary Address Provider machine could be still off at this point
    if (hblSM != NULL) hblSM->onPacketReceived(*packet);
    if (papSM != NULL) papSM->onPacketReceived(*packet);
    
    delete packet; 
}

/**
 * @brief This method must not be called because there are no layers above.
 * If called, it throws an exception.
 *
 * @param type unused
 * @param transportLayer unused
 */
void HypercubeControlLayer::registerTransportProtocol(const TTransportProtocolId &id, TTransportLayer *transportLayer)
{
    throw new logic_error("HypercubeControlLayer::registerTransportProtocol must not be called!");    
}

/**
 * @brief Called when a message is received.
 * 
 * @param message the message received.
 */
void HypercubeControlLayer::onMessageReceived(const TMessage *message)
{
    if (message->getType() == ConnectedMessage::TYPE) {
        const ConnectedMessage *cm = dynamic_cast<const ConnectedMessage *>(message);
        papSM = new PAPSM(this);
        
        if (hbEnabled) hblSM = new HBLSM(this);    
        recoveredMask = HypercubeAddress(cm->getPrimaryAddress().getBitLength());
        
    } else if (message->getType() == DisconnectedMessage::TYPE) {
        delete papSM;
        if (hblSM != NULL) delete hblSM;
        
        papSM = NULL;
        hblSM = NULL;

        addresses.clear();
        neighbours.clear();
    }
        
}

/**
 * @brief Get the neighbour map.
 *
 * @return the neighbour map.
 */
map<MACAddress, Neighbour>  &HypercubeControlLayer::getNeighbours()
{
    return neighbours;
}

/**
 * @brief Returns whether a primary address has already been assigned.
 *
 * @return whether a primary address has already been assigned.
 */
bool HypercubeControlLayer::hasPrimaryAddress() const
{
    return addresses.size() > 0;
}

/**
 * @brief Get the primary address.
 *
 * @return the primary address.
 */
HypercubeMaskAddress HypercubeControlLayer::getPrimaryAddress() const
{
    return addresses.size() > 0? addresses[0] : HypercubeMaskAddress(HypercubeAddress(), 0);
}

/**
 * @brief Get a list of all the node addresses.
 *
 * @return a list of all the node addresses.
 */
const vector<HypercubeMaskAddress> &HypercubeControlLayer::getAddresses() const
{
    return addresses;
}

/**
 * @brief Helper method to send packets through the Data Link Layer.
 *
 * @param dest destination address.
 * @packet packet to send.
 */
void HypercubeControlLayer::send(const MACAddress &dest, const TControlPacket &packet)
{
    Simulator::getInstance()->notify("node.sent.hcpacket." + toLower(packet.getName()), &packet, NULL, getNode());

    packetStats[PACKETS_SENT][packet.getType() - 1]++;
    packetStats[BYTES_SENT][packet.getType() - 1]+= packet.getTotalLength();
    
    dataLinkLayer->send(dest, TControlPacket::ETHERNET_TYPE, packet);
}      

/**
 * @brief Assign a primary address to this node.
 *
 * @param addr Address and mask to assingn
 */
void HypercubeControlLayer::setPrimaryAddress(const HypercubeMaskAddress &addr)
{
    string notif;
    if (addresses.size() == 0) {
        initialMask = addr.getMask();
        addresses.push_back(addr);
        notif = "assigned";
    } else {
        addresses[0] = addr;
        notif = "changed";
    }
    Simulator::getInstance()->notify("node.primaryAddress." + notif, &addr, NULL, getNode());   
}

/**
 * @brief Add a secondary address to the node.
 *
 * @param addr address to add.
 */
void HypercubeControlLayer::addSecondaryAddress(const HypercubeMaskAddress &addr)
{
    addresses.push_back(addr);
    Simulator::getInstance()->notify("node.secondaryAddress.assigned", &addr, NULL, getNode());    
}

/**
 * @brief Returns whether the node has at least a child, i.e. it has given part of its address space.
 *
 * @return whether the node has at least a child.
 */ 
bool HypercubeControlLayer::hasChild() const
{
    return addresses.size() > 0? (initialMask != addresses[0].getMask()) : false;
}

/**
 * @brief Get a list of addresses that can be used for reconnection.
 *
 * @return a list of addresses that can be used for reconnection.
 */
vector<HypercubeMaskAddress> HypercubeControlLayer::getReconnectAddresses() const 
{ 
    return reconnectAddresses; 
}

/**
 * @brief Add an address that can be used for reconnection.
 *
 * @param addr address to add to the reconnection address list.
 */
void HypercubeControlLayer::addReconnectAddress(const HypercubeMaskAddress &addr) 
{ 
    reconnectAddresses.push_back(addr); 
}

/**
 * @brief Erase an address from the reconnection address list.
 *
 * @param addr address to remove from the reconnection address list.
 * @return true if the address was contained in the reconnection addresses list
 */
bool HypercubeControlLayer::eraseReconnectAddress(const HypercubeAddress &addr) 
{ 
    vector<HypercubeMaskAddress>::iterator it= find(reconnectAddresses.begin(), reconnectAddresses.end(), addr); 
    if (it != reconnectAddresses.end()) {
        reconnectAddresses.erase(it);
        return true;
    }
    return false;
} 

bool HypercubeControlLayer::isProposingPrimaryAddress() const
{
    return papSM == NULL? true : papSM->isProposingPrimaryAddress(); 
}

bool HypercubeControlLayer::isProposingSecondaryAddress() const
{
    return hblSM == NULL? false : hblSM->isProposingSecondaryAddress();
}

string HypercubeControlLayer::getPacketStats(StatsType type) const
{
    string s;
    for (int i = 0; i < 8; i++) {
        s += "," + toStr(packetStats[type][i]);
    }
    return s.substr(1);
}

void HypercubeControlLayer::clearPacketStats()
{
    for (int i = 0; i < 8; i++) {
        packetStats[0][i] = 0;
        packetStats[1][i] = 0;
        packetStats[2][i] = 0;
        packetStats[3][i] = 0;
    }
}

void HypercubeControlLayer::setHBEnabled(bool enabled) 
{
     hbEnabled = enabled;

    if(enabled) {
        if (papSM != NULL) { 
           hblSM = new HBLSM(this);
           mainSM->enableHB();
        }
    } else {
        if (hblSM != NULL) delete hblSM;
        hblSM = NULL;
    }   
}


bool HypercubeControlLayer::isHBEnabled() const 
{
    return hbEnabled;
}


}
}

