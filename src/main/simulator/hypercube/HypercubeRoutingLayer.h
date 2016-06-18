#ifndef _HYPERCUBEROUTINGLAYER_H_
#define _HYPERCUBEROUTINGLAYER_H_

#include "common.h"
#include "HCPacket.h"
#include "Units.h"
#include "Event.h"
#include "Frame.h"
#include "Simulator.h"
#include "HypercubeMaskAddress.h"
#include "MACAddress.h"
#include "TRoutingAlgorithm.h"
#include "DataPacket.h"
#include "HypercubeControlLayer.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::hypercube::routing;
using namespace simulator::event;
using namespace simulator;
using namespace simulator::address;

class HypercubeControlLayer;

/**
 * @brief Network layer for the Hypercube Protocol.  
 * It takes care of routing.
 * Upper layers can ask to send a packet, then it will route to the appropiate node.
 * When receiving a packet, if the node is the final destination, it passes to the
 * upper layer, but if it is not, it routes the packet without the upper layer 
 * even knowing that.
 */
class HypercubeRoutingLayer : public TNetworkLayer {
    public:
        HypercubeRoutingLayer(TDataLinkLayer *dll, HypercubeControlLayer *hcl);
        
        virtual void send(const TNetworkAddress &dest, const TTransportProtocolId &protocol, const TSegment &segment);        
        virtual void receive(const TPhysicalAddress &from, const TFrame &frame);
        virtual void registerTransportProtocol(const TTransportProtocolId &id, TTransportLayer *transportLayer);

        TRoutingAlgorithm *getRouting();
        
    private:
        void notifyTraceRoute(const DataPacket &dp, bool hasRoute = true);
        void sendToRoute(const HypercubeAddress &nextHop, DataPacket *dp);
        bool hasArrived(const DataPacket &dp);
        
        /// Map of tranpsort protocols available above this layer.
        map<TransportType, TTransportLayer *> transportProtocols;
        
        /// Routing algorithm used.
        TRoutingAlgorithm *routing;
        
        /// Pointer to the HypercubeControlLayer
        HypercubeControlLayer *hcl;

};
      
}
}

#endif
