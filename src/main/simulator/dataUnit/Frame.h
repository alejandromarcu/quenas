#ifndef _FRAME_H_
#define _FRAME_H_

#include <vector>
#include "DataUnit.h"
#include "common.h"
#include "MACAddress.h"
#include "ProtocolIDs.h"

namespace simulator {
    namespace dataUnit {

using namespace std;
using namespace simulator::address;
using namespace simulator::layer;

       
/**
 * @brief A Frame used for communicating between Data Link Layers.
 *
 * The frame is similar to Ethernet II frame (See http://telecom.tbi.net/frmlan.html)
 */        
class Frame : public TFrame {
    public:
        Frame(const MACAddress &source, const MACAddress &destination, EthernetType ethernetType, const TPacket &packet);
        Frame(const TBitStream &bitStream);

        virtual void dumpTo(BackItVB it) const;

        MACAddress getSource() const;
        MACAddress getDestination() const;
        EthernetType getEthernetType() const;

    private:        
        /// source address of the frame
        MACAddress source;

        /// destination address of the frame
        MACAddress destination;
        
        /// type of packet contained in the payload
        EthernetType ethernetType;

};



}
}

#endif
