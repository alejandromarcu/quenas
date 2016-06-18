#ifndef _HYPERCUBECONTROLLAYER_H_
#define _HYPERCUBECONTROLLAYER_H_

#include <vector>
#include <iterator>

#include "common.h"
#include "HCPacket.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "Frame.h"
#include "Simulator.h"
#include "HypercubeMaskAddress.h"
#include "MACAddress.h"
#include "TRoutingAlgorithm.h"
#include "DataPacket.h"
#include "Neighbour.h"

namespace simulator {
    namespace hypercube {
        
/**
 * @brief Network layer for the Hypercube protocol, in charge of controlling the address
 * connection and disconnection.
 * No layers can be above it.
 */
class HypercubeControlLayer : public TNetworkLayer, public TMessageReceiver {
    public:
        typedef enum StatsType { PACKETS_SENT, PACKETS_RECEIVED, BYTES_SENT, BYTES_RECEIVED };


        HypercubeControlLayer(TNode *node, TDataLinkLayer *dll);
        virtual void send(const TNetworkAddress &dest, const TTransportProtocolId &protocol, const TSegment &segment);        
        virtual void receive(const TPhysicalAddress &from, const TFrame &frame);
        virtual void registerTransportProtocol(const TTransportProtocolId &id, TTransportLayer *transportLayer);
        virtual void onMessageReceived(const TMessage *message);
        map<MACAddress, Neighbour> &getNeighbours();

        HypercubeMaskAddress getPrimaryAddress() const;
        const vector<HypercubeMaskAddress> &getAddresses() const;
    
        void setPrimaryAddress(const HypercubeMaskAddress &addr);    
        void addSecondaryAddress(const HypercubeMaskAddress &addr);    
        
        void send(const MACAddress &dest, const TControlPacket &packet);
        bool hasPrimaryAddress() const;

        HypercubeAddress getRecoveredMask() const;
        void setRecoveredMask(const HypercubeAddress &rm);        
        
        vector<HypercubeMaskAddress> getReconnectAddresses() const;
        void addReconnectAddress(const HypercubeMaskAddress &addr);
        bool eraseReconnectAddress(const HypercubeAddress &addr);
                
        bool hasChild() const;
        
        bool isProposingPrimaryAddress() const;
        bool isProposingSecondaryAddress() const;

        string getPacketStats(StatsType type) const;
        void clearPacketStats();

        void setHBEnabled(bool enabled);
        bool isHBEnabled() const;
    private:
        /// Addresses of the node, the first is the primary address, the rest are secondary addresses
        vector<HypercubeMaskAddress> addresses;
        
        /// Addresses that can be used for reconnection
        vector<HypercubeMaskAddress> reconnectAddresses;
        
        /// Main state machine
        MainSM *mainSM;
        
        /// Heard Bit Listener State Machine
        HBLSM *hblSM;
        
        /// Primary Address Provider State Machine
        PAPSM *papSM;
        
        /// Neighbours of the node
        map<MACAddress, Neighbour> neighbours;
        
        /// Which bits of the space where recovered, used to defragment space
        HypercubeAddress recoveredMask;

        /// The mask of the primary address that the node got when it was just connected.
        int initialMask;

        /// Counts the number of packets/bytes sent/received by type
        long packetStats[4][8];

        /// Whether Heard Bit sending is enabled
        bool hbEnabled;
};

      
}
}

#endif
