#include <vector>
#include "Frame.h"
#include "common.h"
#include "MACAddress.h"
#include "ProtocolIDs.h"

namespace simulator {
    namespace dataUnit {

using namespace std;
using namespace simulator::address;
using namespace simulator::layer;

/**
 * @brief Creates a Frame from a packet and some additional information.
 *
 * @param source address of the frame source.
 * @param destination address of the frame destination.         
 * @param ethernetType type of packet contained in the payload
 * @param packet the packet transported in the frame
 */
Frame::Frame(const MACAddress &source, const MACAddress &destination,  
      EthernetType ethernetType, const TPacket &packet) 
    : source(source), destination(destination), ethernetType(ethernetType)
{
    packet.dumpTo(back_inserter(data));
}

   
/**
 * @brief  Extracts a frame from a bit stream
 * 
 * @param bitStream the bit stream to extract the frame from.
 */
Frame::Frame(const TBitStream &bitStream) 
{
    ConstItVB it = bitStream.getPayload().begin();

    byte aux[6];
    
    // First 6 bytes are destination MAC Address
    copy (it, it + 6, aux);
    destination = MACAddress(aux);

    // Following 6 bytes are source MAC Address
    copy (it + 6, it + 12, aux);
    source = MACAddress(aux);

    it += 12;            

    // Following 2 bytes are the ethernet type
    ethernetType = readInt16(it);

    // Remaining bytes are the payload
    copy(it, bitStream.getPayload().end(), back_inserter(data)); 
}
    
/**
 * @brief Dumps this frame to an iterator, including its header and payload.
 *
 * The Format is:
 *    - 6 bytes of MAC destination address
 *    - 6 bytes of MAC source address
 *    - 2 bytes of ethernet type
 *    - data bytes
 *
 * @param it the back iterator where the data is inserted.
 */
void Frame::dumpTo(BackItVB it) const 
{
    destination.dumpTo(it);
    source.dumpTo(it);
    
    dumpInt16To(ethernetType.getType(), it);
    
    copy(data.begin(), data.end(), it);
};        

/**
 * @brief Returns the source address of the frame
 * 
 * @return the source address of the frame
 */
MACAddress Frame::getSource() const 
{
    return source;
}

/**
 * @brief Returns the destination address of the frame
 * 
 * @return the destination address of the frame
 */
MACAddress Frame::getDestination() const 
{
    return destination;
}

/**
 * @brief Returns the Ethernet Type of the frame
 * 
 * @return the Ethernet Type of the frame
 */
EthernetType Frame::getEthernetType() const
{
    return ethernetType;
}



}
}

