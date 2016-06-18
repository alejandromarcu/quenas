#ifndef _PHYSICALLAYER_H_
#define _PHYSICALLAYER_H_

#include <vector>
#include <map>

#include "DataUnit.h"
#include "TPhysicalAddress.h"
#include "TPhysicalLayer.h"
#include "MACAddress.h"
#include "Units.h"
#include "TDataLinkLayer.h"

namespace simulator {
    namespace layer {

using namespace std;
using namespace simulator::dataUnit;
using namespace simulator::address;

class TConnection;

/**
 * @brief Represents a simple Physical Layer that uses MAC addresses.
 */
class PhysicalLayer : public TPhysicalLayer {
    public:
        PhysicalLayer(TNode *node, const MACAddress &address);
        ~PhysicalLayer();        

        virtual void send(const TPhysicalAddress &dest, const TFrame &frame);        
        virtual void receive(const TBitStream &bitStream);
        
        void setDataLinkLayer(TDataLinkLayer *dataLinkLayer);
        
        virtual void addConnection(TConnection *conn);
        virtual void removeConnection(TConnection *conn);        
        
        virtual const MACAddress &getAddress() const;

        map<MACAddress, TConnection*> &getConnections();
    private:
        void sendBroadcast(const TFrame &frame);
        
        /// Address associated with this physical layer.
        MACAddress address;
        
        /// Connections available.
        map<MACAddress, TConnection*> connections;
        
        /// Time when the connections will have finished transmiting data
        /// and will be available to send again.
        Time nextTimeToSend;        
        
        /// Pointer to the Data Link Layer.
        TDataLinkLayer *dataLinkLayer;            
};

}}

#endif
