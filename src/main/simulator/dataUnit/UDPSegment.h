#ifndef _UDPSEGMENT_H_
#define _UDPSEGMENT_H_

#include <vector>
#include "DataUnit.h"
#include "ProtocolIDs.h"
#include "common.h"

using namespace simulator::layer;

namespace simulator {
    namespace dataUnit {

using namespace std;
        
/**
 * @brief An UDP segment used for communicating between transport layers.
 */        
class UDPSegment : public TSegment {
    public:
        UDPSegment(int16 sourcePort, int16 destinationPort, const TData& data);
        UDPSegment(const TPacket &packet);
        virtual void dumpTo(BackItVB it) const;
        int16 getSourcePort() const;
        int16 getDestinationPort() const;

    private:        
        /// source port for the udp segment
        int16 sourcePort;

        /// destination port for the udp segment
        int16 destinationPort;
};



}
}

#endif
