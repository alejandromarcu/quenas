#ifndef _TAPPLICATIONLAYER_H_
#define _TAPPLICATIONLAYER_H_

#include "DataUnit.h"
#include "TNetworkAddress.h"
#include "TPhysicalAddress.h"
#include "TLayer.h"
#include "common.h"
#include "ProtocolIDs.h"
#include "Data.h"

namespace simulator {
    namespace layer {

using namespace simulator::dataUnit;
using namespace simulator::address;

class TTransportLayer;

/**
 * @brief Base class for all Application classes.
 */
class TApplicationLayer : public TLayer {
    public:
        /**
         * @brief Create a new application layer.
         *
         * @param node pointer to the node containing this application.
         * @param tl pointer to the transport layer used by this application.
         */                
        TApplicationLayer(TNode *node, TTransportLayer *tl) : TLayer (node), transportLayer(tl) {};
        virtual ~TApplicationLayer() {};
      
        /**
         * @brief This method is called when data is received.
         *
         * @param from network address that sent the data.
         * @param sourceAppId the appplication id of the application sending the data.
         * @param data data sent.
         */      
        virtual void receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet) = 0;

        /**
         * @brief Return the application id where this application is binded.
         *
         * @return the application id where this application is binded.
         */
        virtual TApplicationId getApplicationId() const = 0 ;

    protected:        
        /// Pointer to the transport layer used by this application.
        TTransportLayer *transportLayer;

};

}
}
    
    
#endif    
