#include <vector>
#include "DataUnit.h"
#include "ProtocolIDs.h"
#include "common.h"
#include "UDPSegment.h"

using namespace simulator::layer;

namespace simulator {
    namespace dataUnit {

using namespace std;
        
/** 
 * @brief Creates an UDP segment from the source and destination port
 * and the data it carries.
 *
 * @param sourcePort source port for the segment
 * @param destinationPort destination port for the segment
 * @param data data that the segment carries
 */
UDPSegment::UDPSegment(int16 sourcePort, int16 destinationPort, const TData& data) :
    sourcePort(sourcePort), destinationPort(destinationPort)
{
    data.dumpTo(back_inserter(this->data));            
}
   
/**
 * @brief Creates an UDP segment from a packet.
 *
 * It gets the payload of the packet, and rebuilds the segment from
 * the raw bytes.
 *
 * @param packet the packet to build the segment from
 */             
UDPSegment::UDPSegment(const TPacket &packet) {
    VB::const_iterator it = packet.getPayload().begin(); 

    sourcePort = readInt16(it);
    destinationPort = readInt16(it);            
    int16 length = readInt16(it);
    
    readInt16(it); // skip checksum

    copy(it, it + (length - 8), back_inserter(data));
    
}               

/**
 * @brief Dumps this segment to an iterator, including its header and payload.
 *
 * @param it the back iterator where the data is inserted.
 */
void UDPSegment::dumpTo(BackItVB it) const 
{
    dumpInt16To(sourcePort, it);
    dumpInt16To(destinationPort, it);
    
    // UDP length: length of payload plus header
    dumpInt16To(data.size() + 8, it);
    
    // Checksum: not implemented
    dumpInt16To(0, it);
    
    copy(data.begin(), data.end(), it);
};        

/**
 * @brief Retrieve the source port for this segment.
 *
 * @return the source port for this segment.
 */         
int16 UDPSegment::getSourcePort() const 
{
    return sourcePort;
}

/**
 * @brief Retrieve the destination port for this segment.
 *
 * @return the destination port for this segment.
 */         
int16 UDPSegment::getDestinationPort() const 
{
    return destinationPort;
}


}
}
