#ifndef _TNETWORKLAYER_H_
#define _TNETWORKLAYER_H_

#include <vector>
#include "DataUnit.h"
#include "TNetworkAddress.h"
#include "TLayer.h"
#include "common.h"
#include "ProtocolIDs.h"
#include "TTransportLayer.h"

namespace simulator {
    namespace layer {

using namespace simulator::dataUnit;
using namespace simulator::address;

class TDataLinkLayer;

/**
 * @brief Base class for all the network layers.
 */
class TNetworkLayer : public TLayer {
    public:
        /**
         * @brief Create a new network layer.
         *
         * @param node pointer to the node containing this layer.
         * @param dll pointer to the data link layer used by this layer.
         */
        TNetworkLayer(TNode *node, TDataLinkLayer *dll) : TLayer (node), dataLinkLayer(dll) {};
               
        /**
         * @brief Send a segment to the specified destination.
         *
         * @param dest destination network address.
         * @param protocol id of the transport protocol that must handle the segment.
         * @param segment segment to send.
         */         
        virtual void send(const TNetworkAddress &dest, const TTransportProtocolId &protocol, const TSegment &segment) = 0;        
        
        /**
         * @brief This method is called when a frame is received.
         *
         * @param from the physical address from which this segment was sent.
         * @param frame the received frame.
         */
        virtual void receive(const TPhysicalAddress &from, const TFrame &frame) = 0;
        
        /**
         * @brief Register or unregister (if the pointer is NULL) a transport protocol.
         *
         * @param type id of the transport protocol to register
         * @param transportLayer pointer to the transport layer that handles the protocol, or NULL to unregister it.
         */
        virtual void registerTransportProtocol(const TTransportProtocolId &id, TTransportLayer *transportLayer) = 0;
               
        virtual TDataLinkLayer *getDataLinkLayer() { return dataLinkLayer; }
    protected:
        /// Data Link layer used by this network layer.
        TDataLinkLayer *dataLinkLayer;
};

}
}
    
    
#endif    
