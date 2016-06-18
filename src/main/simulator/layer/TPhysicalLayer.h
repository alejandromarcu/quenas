#ifndef _TPHYSICALLAYER_H_
#define _TPHYSICALLAYER_H_

#include <vector>
#include "DataUnit.h"
#include "TPhysicalAddress.h"
#include "TLayer.h"
#include "common.h"

namespace simulator {
    namespace layer {

using namespace simulator::dataUnit;
using namespace simulator::address;

class TConnection;

/**
 * @brief Base class for Physical Layer implementations.
 *
 * A Physical Layer is connected to other similar layes through TConnection objects.
 * It can send frames or receive bit streams from another physical layer.
 */
class TPhysicalLayer : public TLayer {
    public:
        /**
         * @brief Create a TPhysicalLayer. Just stores the node pointer.
         */
        TPhysicalLayer(TNode *node) : TLayer(node) {};
        
        /**
         * @brief Virtual empty destructor.
         */
        virtual ~TPhysicalLayer() {};
        
        /**
         * @brief Return the address associated with this layer.
         * 
         * @return the address associated with this layer.
         */
        virtual const TPhysicalAddress &getAddress() const = 0;

    
        /**
         * @brief Add a connection linking this layer to another physical layer.
         *
         * @param conn a connection object that connects this physical layer to other.
         */
        virtual void addConnection(TConnection *conn) = 0;

        /**
         * @brief Remove a connection linking this layer to another physical layer.
         *
         * @param conn a connection object that connects this physical layer to other.
         */
        virtual void removeConnection(TConnection *conn) = 0;        
        
        /**
         * @brief Send a frame to another physical layer that is connected to this one.
         *
         * @param dest physical address destination for the frame.
         * @param frame the frame to send.
         */
        virtual void send(const TPhysicalAddress &dest, const TFrame &frame) = 0;        
        
        /**
         * @brief Receive a bitStream from another physical layer.
         * 
         * @param bitStream the bitStream received.
         */
        virtual void receive(const TBitStream &bitStream) = 0;
                    
};

}
}

#endif
