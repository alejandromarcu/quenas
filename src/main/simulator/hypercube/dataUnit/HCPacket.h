#ifndef _HCPACKET_H_
#define _HCPACKET_H_

#include <vector>
#include <iterator>

#include "common.h"
#include "DataUnit.h"
#include "MACAddress.h"
#include "HypercubeAddress.h"
#include "HypercubeMaskAddress.h"
#include "ProtocolIDs.h"
#include "Command.h"
#include "TOptionalHeader.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {

using namespace std;
using namespace simulator::dataUnit;
using namespace simulator::address;
using namespace simulator::layer;
using namespace simulator::command;

       
/**
 * @brief Optional header used to propose an address to a node.
 */        
class AdditionalAddress : public TOptionalHeader {
    public:
        /// Type of header
        const static byte HEADER_TYPE;
        
        AdditionalAddress();        
        AdditionalAddress(const HypercubeAddress &address);        
        AdditionalAddress(const HypercubeAddress &address, byte mask);        
        AdditionalAddress(const HypercubeAddress &address, byte mask, byte connectionCount);        
        
        AdditionalAddress(ConstItVB &it);
        
        HypercubeMaskAddress getMaskAddress() const;
        HypercubeAddress getAddress() const;
        byte getMask() const;
        byte getConnectionCount() const;
        
        void setMask(byte mask);
        void setConnectionCount(byte connectionCount);
        
        bool hasMask() const;
        bool hasConnectionCount() const;
        
        virtual void dumpTo(BackItVB it) const;

        virtual QueryResult *query(const vector<string> *options = NULL) const;
    private:
        /// Proposed address
        HypercubeAddress address;
        
        /// Mask for the proposed address
        byte mask;
        
        /// Number of connections that the node has
        byte connectionCount;
};


/**
 * @brief Base class for all Hypercube Control packets.
 */        
class TControlPacket : public TPacket, public TCommandRunner, public TQueryable {
    public:        
        /// Ethernet Type protocol number        
        const static EthernetType ETHERNET_TYPE;
        
        virtual ~TControlPacket();
        
        byte getType() const;
        MACAddress getPhysicalAddress() const;
        HypercubeMaskAddress getPrimaryAddress() const;
        
        virtual void dumpTo(BackItVB it) const;
        
        
        static TControlPacket *create(const TFrame &frame);
        
        virtual TCommandResult *runCommand(const Function &function);
        virtual void queryAdditionalFields(QueryResult *qr) const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;    

        virtual void read(const TFrame &frame);
        virtual byte getTotalLength() const;
        
    protected:
        TControlPacket();
        TControlPacket(byte type, const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress);
        
        bool getFlag(int n) const;
        void setFlag(int n, bool value);
        virtual void addOptionalHeader(AdditionalAddress header);

        virtual void dumpOptionalHeadersTo(BackItVB it) const;
        
    protected:
        /// Total length of the packet
        byte totalLength;
        
    private:
        /// Type of header (5 bits) and flags (3 bits)
        byte typeAndFlags;        
        
        /// Physical Address of the sending node
        MACAddress physicalAddress;
        
        /// Primary Address of the sending node.
        HypercubeMaskAddress primaryAddress;
};

/**
 * @brief Primary Address Request packet.
 * It is used to ask for a Primary Address to all neighbours (Broadcast).
 */
class PARPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;

        PARPacket();
        PARPacket(const MACAddress &physicalAddress);        
        virtual string getName() const;

};

/**
 * @brief Primary Address Proposal packet.
 * It is used to reply a PAR packet, proposing an address (Unicast).
 */
class PAPPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;

        PAPPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress);
        PAPPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress, 
                  const AdditionalAddress &proposed, 
                  const vector<AdditionalAddress> &reconnect = vector<AdditionalAddress>());                 

        PAPPacket();
        virtual string getName() const;
        
        bool isAddressExhausted() const;
        AdditionalAddress getProposed() const;
        vector<AdditionalAddress> getReconnect() const;
        virtual void queryAdditionalFields(QueryResult *qr) const;
        
    protected:
        virtual void dumpOptionalHeadersTo(BackItVB it) const;
        virtual void addOptionalHeader(AdditionalAddress header);

    private:
        /// The primary address proposed
        AdditionalAddress proposed;
        
        /// Reconnection addresses proposed.
        vector<AdditionalAddress> reconnect;
        
        
};

/**
 * @brief Primary Address Notification packet.
 * Is sent to notify of the choosen primary address (Broadcast).
 */
class PANPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;
        
        PANPacket();
        PANPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress);
        virtual string getName() const;
        
};


/**
 * @brief Primary Address Notification Confirmation packet.
 * Is used to confirm that a PAN packet has correctly arrived (Unicast).
 */
class PANCPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;

        PANCPacket();
        PANCPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress);
        virtual string getName() const;
             
};

/**
 * @brief Disconnection packet.
 * This packet informs that the node will disconnect (Broadcast).
 */
class DiscPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;

        DiscPacket();
        DiscPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress, bool hasChild); 
        
        bool hasChild() const;
        virtual string getName() const;        
};

/**
 * @brief Heard Bit packet.
 * Is sent to inform that the node is still alive and connected (Broadcast).
 */
class HBPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;
        
        HBPacket();
        HBPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress);
        virtual string getName() const;
        
};

/**
 * @brief Secondary Address Proposal packet.
 * Is sent to propose a secondary address for a node (Unicast)
 */
class SAPPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;

        SAPPacket();
        SAPPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress, const AdditionalAddress &proposed);

        AdditionalAddress getProposed() const;
        virtual string getName() const;

    protected:
        virtual void addOptionalHeader(AdditionalAddress header);
        virtual void dumpOptionalHeadersTo(BackItVB it) const;

    private:
        /// Proposed secondary address.
        AdditionalAddress proposed;

};

/**
 * @brief Secondary Address Notification.
 * Is used to accept or reject an address proposed by a SAP packet.
 */
class SANPacket : public TControlPacket {
    public:
        /// Type of packet
        const static byte TYPE;

        SANPacket();
        SANPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress,
                    bool accepted, AdditionalAddress proposed);

        AdditionalAddress getProposed() const;
        bool isAccepted() const;

        virtual string getName() const;
        virtual void queryAdditionalFields(QueryResult *qr) const;
        
    protected:
        virtual void addOptionalHeader(AdditionalAddress header);
        virtual void dumpOptionalHeadersTo(BackItVB it) const;
        
    private:
        /// Proposed secondary address that is being rejected or accepted.
        AdditionalAddress proposed;

};

}
}
}

#endif
