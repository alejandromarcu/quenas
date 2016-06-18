#ifndef _TRANSPORTLAYER_H_
#define _TRANSPORTLAYER_H_

#include <map>

#include "TTransportLayer.h"
#include "DataUnit.h"
#include "TPhysicalAddress.h"
#include "TLayer.h"
#include "common.h"
#include "ProtocolIDs.h"
#include "TApplicationLayer.h"
#include "Data.h"

namespace simulator {
    namespace layer {

using namespace simulator::dataUnit;
using namespace simulator::address;

using namespace std;

class TNetworkLayer;

/**
 * @brief Simplified Transport Layer for the UDP protocol.
 */
class UDPTransportLayer : public TTransportLayer {
    public:
        UDPTransportLayer(TNode *node, TNetworkLayer *nl);
        
        virtual void bind(TApplicationLayer *app);
        
        virtual void bind(const TApplicationId &appId, TApplicationLayer *app);
        
        virtual void send(const TNetworkAddress &dest, const TApplicationId &sourceAppId,
                          const TApplicationId &destAppId, const TData &data);

        virtual void receive(const TNetworkAddress &from, const TPacket &packet);
        
        virtual TApplicationLayer* getApplication(const TApplicationId &appId);

        
    protected:
        /// Map of the ports with its binded application layers.
        map<Port, TApplicationLayer*> applications;
};

}
}
    
    
#endif    
