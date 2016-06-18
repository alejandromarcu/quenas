#ifndef _TDATALINKLAYER_H_
#define _TDATALINKLAYER_H_

#include <vector>
#include "DataUnit.h"
#include "TPhysicalAddress.h"
#include "TPhysicalLayer.h"
#include "TNetworkLayer.h"
#include "TLayer.h"
#include "common.h"
#include "ProtocolIDs.h"

namespace simulator {
    namespace layer {

using namespace simulator::dataUnit;
using namespace simulator::address;

/**
 * @brief Base class for Data Link Layer classes.
 */
class TDataLinkLayer : public TLayer {
    public:
        /**
         * @brief Create a new Data Link Layer.
         *
         * @param node pointer to the node containing this layer.
         * @param physicalLayer pointer to the physical layer attached to this layer.
         */
        TDataLinkLayer(TNode *node, TPhysicalLayer *physicalLayer) : TLayer (node), physicalLayer(physicalLayer) {};

        /**
         * @brief Send a packet to the specified physical address, that will be retrieved in the other end
         * by a network layer handling the specified protocol.
         *
         * @param dest destination address
         * @param protocol network protocol that must handle the packet.
         * @param packet the packet to send.
         */                 
        virtual void send(const TPhysicalAddress &dest, const TNetworkProtocolId &protocol, const TPacket &packet) = 0;
        
        /**
         * @brief This method is called when a bitStream is received by a physical layer.
         *
         * @param bitStream the received bit stream.
         */
        virtual void receive(const TBitStream &bitStream) = 0;
        
        /**
         * @brief Register or unregister (if the pointer is NULL) a network protocol.
         *
         * @param type id of the network protocol to register
         * @param networkLayer pointer to the network layer that handles the protocol, or NULL to unregister it.
         */
        virtual void registerNetworkProtocol(const TNetworkProtocolId &type, TNetworkLayer *networkLayer) = 0;

    protected:
        
        /// Pointer to the physical layer attached to this layer.
        TPhysicalLayer *physicalLayer;
};

}
}
    
    
#endif    
