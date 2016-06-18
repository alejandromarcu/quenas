#ifndef _MOCK_LAYER_H
#define _MOCK_LAYER_H

#include <vector>
#include "DataUnit.h"
#include "TPhysicalAddress.h"
#include "TPhysicalLayer.h"
#include "TDataLinkLayer.h"
#include "TLayer.h"
#include "common.h"
#include "MACAddress.h"
#include "BitStream.h"
#include "Units.h"
#include "Simulator.h"
#include "TNetworkLayer.h"
#include "Frame.h"
#include "MockDataUnit.h"

namespace simulator {
    namespace layer {
        namespace tests {
            
using namespace simulator::dataUnit;
using namespace simulator::dataUnit::tests;
using namespace simulator::address;

/**
 * @brief Mock Physical Layer used for testing.
 */
class MockPhysicalLayer : public TPhysicalLayer {
    public:
        MockPhysicalLayer(TNode *node, MACAddress address) : TPhysicalLayer(node), address(address) {
            received = NULL;
        }
        
        virtual ~MockPhysicalLayer() {
            if (received != NULL) delete received;
        }
        
        virtual const TPhysicalAddress &getAddress() const 
        {
            return address;
        }

        virtual void addConnection(TConnection *conn) {}
        virtual void removeConnection(TConnection *conn) {}
        virtual void send(const TPhysicalAddress &dest, const TFrame &frame) {}
        virtual void receive(const TBitStream &bitStream) {
            
            if (received != NULL) delete received;            
            received = new BitStream(dynamic_cast<const BitStream& >(bitStream));
            timeReceived = Simulator::getInstance()->getTime();
        }
        
        BitStream *getReceived() {
            return received;
        }
        
        Time getTimeReceived() {
            return timeReceived;
        }
        
    private:
        MACAddress address;
        BitStream *received;
        Time timeReceived;
        
                    
};

/**
 * @brief Mock DataLinkLayer used for testing.
 */
class MockDataLinkLayer : public TDataLinkLayer {
    public:
        MockDataLinkLayer(TNode *node, TPhysicalLayer *physicalLayer) : TDataLinkLayer (node, physicalLayer) {
            received = NULL;
            timeReceived = -1;
        };
        
        virtual void receive(const TBitStream &bitStream) {            
            if (received != NULL) delete received;            
            received = new BitStream(dynamic_cast<const BitStream& >(bitStream));
            timeReceived = Simulator::getInstance()->getTime();
        }
        
        BitStream *getReceived() {
            return received;
        }
        
        Time getTimeReceived() {
            return timeReceived;
        }

        virtual void send(const TPhysicalAddress &dest, const TNetworkProtocolId &protocol, const TPacket &packet)  {};
        virtual void registerNetworkProtocol(const TNetworkProtocolId &type, TNetworkLayer *networkLayer) {};

    private:
        BitStream *received;
        Time timeReceived;

};


/**
 * @brief Mock Network Layer for testing.
 */
class MockNetworkLayer : public TNetworkLayer {
    public:
        MockNetworkLayer(TNode *node, TDataLinkLayer *dll) : TNetworkLayer (node, dll) {
            received = NULL;
            from = NULL;
        };

        virtual void send(const TNetworkAddress &dest, const TTransportProtocolId &protocol, const TSegment &segment) {};
        virtual void registerTransportProtocol(const TTransportProtocolId &id, TTransportLayer *transportLayer) {};
        virtual TNetworkProtocolId getProtocolId() const { return EthernetType(0); }
        
        virtual void receive(const TPhysicalAddress &from, const TFrame &frame) {            
            reset();
            received = new Frame(dynamic_cast<const Frame &>(frame));
            this->from = new MACAddress(dynamic_cast<const MACAddress &>(from));
        }
        
        Frame *getReceived() {
            return received;
        }
        
        TPhysicalAddress *getFrom() {
            return from;
        }

        void reset () {
            if (received != NULL) {
                delete received;            
                delete this->from;
                received = NULL;
                from = NULL;
            }
        }
        
    private:
        Frame *received;
        TPhysicalAddress *from;

};

/**
 * @brief Mock Network Address for testing.
 */
class MockNetworkAddress : public TNetworkAddress {
    public:       
        MockNetworkAddress(byte *addr) : TNetworkAddress(addr, 6) {};
        
        MockNetworkAddress(const TAddress &mac) {
            mac.dumpTo(back_inserter(address));
        }
        
        MACAddress getAsMAC() const {
            byte addr[6];
            for (int i=0; i < 6; i++) addr[i] = address[i];
            return MACAddress(addr);
        }

        virtual string toString() const {
            char aux[20];
            sprintf(aux, "%02X:%02X:%02X:%02X:%02X:%02X", address[0], address[1],
                 address[2], address[3], address[4], address[5]);
            
            return aux;
        }
        
    
};

/**
 * @brief Mock Network Layer for testing.
 */
class TrivialNetworkLayer : public TNetworkLayer {
    public:
        TrivialNetworkLayer(TNode *node, TDataLinkLayer *dll) : TNetworkLayer (node, dll) {};

        virtual void send(const TNetworkAddress &dest, const TTransportProtocolId &protocol, const TSegment &segment) 
        {
            dataLinkLayer->send(dynamic_cast<const MockNetworkAddress &>(dest).getAsMAC(),
                                getProtocolId(),
                                MockPacket(segment.getPayload()));
        };

        virtual void registerTransportProtocol(const TTransportProtocolId &id, TTransportLayer *transportLayer) {
            tl = transportLayer;
        };
        virtual TNetworkProtocolId getProtocolId() const { return EthernetType(111); }
        
        virtual void receive(const TPhysicalAddress &from, const TFrame &frame) {            
            tl->receive(MockNetworkAddress(from), MockPacket(frame.getPayload()));
        }
               
    private:
        TTransportLayer *tl;

};


}
}
}

#endif
