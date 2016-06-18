#ifndef _TTRANSPORTLAYER_H_
#define _TTRANSPORTLAYER_H_

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

class TNetworkLayer;

/**
 * @brief Base class for all Transport Layer classes.
 */
class TTransportLayer : public TLayer {
    public:
        /**
         * @brief Create a new transport layer.
         *
         * @param node pointer to the node containing this layer.
         * @param nl pointer to the network layer used by this layer.
         */        
        TTransportLayer(TNode *node, TNetworkLayer *nl) : TLayer (node), networkLayer(nl) {};
        
        /**
         * @brief Bind an application layer to the first Application ID available.
         * The onBind method in the application layer is called to notify the id used.
         *
         * @param app pointer to the application layer to be binded.
         */
        virtual void bind(TApplicationLayer *app) = 0;

        /**
         * @brief Bind an application layer to the specified Application ID.
         * The onBind method in the application layer is called to notify the id used.
         *
         * @param appId application id where the application layer is binded.
         * @param app pointer to the application layer to be binded.
         */
        virtual void bind(const TApplicationId &appId, TApplicationLayer *app) = 0;
        
        /**
         * @brief Send data to the specified network address.
         *
         * @param dest destination address.
         * @param sourceAppId application id of the sending application.
         * @param destAppId application id of the destination application.
         * @param data data to send
         */
        virtual void send(const TNetworkAddress &dest, const TApplicationId &sourceAppId,
                          const TApplicationId &destAppId, const TData &data) = 0;

        /**
         * @brief This method is called when a packet is received.
         *
         * @param from network address that sent the packet.
         * @param packet the packet received.
         */
        virtual void receive(const TNetworkAddress &from, const TPacket &packet) = 0;
        
        
        /**
         * @brief Return a pointer to the application layer binded to the application id or null if not found.
         *
         * @brief appId application id to lookup.
         * @return a pointer to the application layer binded to the application id or null if not found.
         */
        virtual TApplicationLayer* getApplication(const TApplicationId &appId) = 0;

        
    protected:
        /// Pointer to the network layer used by this layer.
        TNetworkLayer *networkLayer;
};

}
}
    
    
#endif    
