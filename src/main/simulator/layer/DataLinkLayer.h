#ifndef _TATALINKLAYER_H_
#define _TATALINKLAYER_H_

#include <map>
#include "DataUnit.h"
#include "TDataLinkLayer.h"
#include "TPhysicalAddress.h"
#include "TNetworkLayer.h"
#include "DataLinkLayer.h"
#include "TLayer.h"
#include "common.h"
#include "ProtocolIDs.h"

namespace simulator {
    namespace layer {

using namespace std;
using namespace simulator::dataUnit;
using namespace simulator::address;

/**
 * @brief Data Link Layer that works with frames of type Frame (Ethernet II). It uses MAC addresses and 
 * multiplexes the network layer used by ethernet type.
 */
class DataLinkLayer : public TDataLinkLayer {
    public:
        DataLinkLayer(TNode *node, TPhysicalLayer *physicalLayer);
        
        virtual void send(const TPhysicalAddress &dest, const TNetworkProtocolId &protocol, const TPacket &packet);
        virtual void receive(const TBitStream &bitStream);
        virtual void registerNetworkProtocol(const TNetworkProtocolId &type, TNetworkLayer *networkLayer);

    private:
        /// Map of available network protocols
        map<EthernetType, TNetworkLayer*> networkProtocols;
        
};

}
}
    
    
#endif    
