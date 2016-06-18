#include <iostream>
#include <cmath>

#include "TPhysicalAddress.h"
#include "DataLinkLayer.h"
#include "MACAddress.h"
#include "BitStream.h"
#include "Connection.h"
#include "Units.h"
#include "Simulator.h"
#include "Frame.h"

namespace simulator {
    namespace layer {

using namespace std;
using namespace simulator::dataUnit;

/**
 * @brief Create a new Data Link Layer.
 *
 * @param node pointer to the node containing this layer.
 * @param physicalLayer pointer to the physical layer attached to this layer.
 */
DataLinkLayer::DataLinkLayer(TNode *node, TPhysicalLayer *physicalLayer) : TDataLinkLayer(node, physicalLayer)
{
    // nothing else to do here
}
        
/**
 * @brief Send a packet to the specified physical address, that will be retrieved in the other end
 * by a network layer handling the specified protocol.
 *
 * @param dest destination address
 * @param protocol network protocol that must handle the packet.
 * @param packet the packet to send.
 */                        
void DataLinkLayer::send(const TPhysicalAddress &dest, const TNetworkProtocolId &protocol, const TPacket &packet)
{
    Frame frame(dynamic_cast<const MACAddress &>(physicalLayer->getAddress()), 
                dynamic_cast<const MACAddress &>(dest), 
                dynamic_cast<const EthernetType &>(protocol), 
                packet);
                
    physicalLayer->send(dest, frame);
}

/**
 * @brief This method is called when a bitStream is received by a physical layer.
 *
 * It creates a frame from the bit stream, chooses the appropiate network layer depending
 * on the ethernet type and calls it receive method.
 *
 * @param bitStream the received bit stream.
 */
void DataLinkLayer::receive(const TBitStream &bitStream)
{
    Frame frame(bitStream);
    
    EthernetType type = frame.getEthernetType();
    
    TNetworkLayer *nl = networkProtocols[type];
    
    if (nl == NULL) throw ("Unknown network protocol type " + type.getType());
    
    nl->receive(frame.getSource(), frame);
}
        
/**
 * @brief Register or unregister (if the pointer is NULL) a network protocol.
 *
 * @param type id of the network protocol to register
 * @param networkLayer pointer to the network layer that handles the protocol, or NULL to unregister it.
 */
void DataLinkLayer::registerNetworkProtocol(const TNetworkProtocolId &type, TNetworkLayer *networkLayer)
{
    EthernetType et = dynamic_cast<const EthernetType &>(type);
    
    if (networkLayer == NULL)  {
        networkProtocols.erase(networkProtocols.find(et));
    } else {
        networkProtocols[et] = networkLayer;
    }
        
}


}
}
