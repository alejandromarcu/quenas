#include "HCPacket.h"
#include "ProtocolIDs.h"
#include "RouteHeader.h"
#include "DataPacket.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {

using namespace std;
using namespace simulator::layer;


const int16 DataPacket::MAX_TTL= 10000;

//-----------------------------------------------------------------------
//-----------------------------< DataPacket >-----------------------------
//-----------------------------------------------------------------------
/**
 * @brief Create a Data Packet carrying a segment.
 *
 * @param source source address of the packet
 * @param dest destination address of the packet
 * @param transportProtocol transport protocol for hte packet
 * @param segment the segment that this data packet carries.
 * @param ttl ttl of the packet
 * @param returned whether this packet is being routed as returned.
 */
DataPacket::DataPacket(const HypercubeAddress &source, const HypercubeAddress &dest,
                            TransportType transportProtocol, const TSegment &segment,
                            int16 ttl, bool returned) 
                : source(source), dest(dest), protocol(transportProtocol), ttl(ttl)
{
    segment.dumpTo(back_inserter(data));
    totalLength = data.size() + 7 + 2 * ((source.getBitLength() + 7) / 8);
    setReturned(returned);
    setRendezVous(false);
    setTraceRoute(false);
}

/**
 * @brief Empty constructor.  Should use read to fill the data.
 */
DataPacket::DataPacket() 
{
}


/** 
 * @brief Virtual Destructor. Does nothing.
 */
DataPacket::~DataPacket()
{
}

/**
 * @brief Read a packet from a frame.
 *
 * @param frame the frame where the packet is read.
 */
void DataPacket::read(const TFrame &frame) 
{
    ConstItVB it = frame.getPayload().begin();
    byte aux[256];
    byte bitLength;
    
    totalLength  = readInt16(it);
    flags       = readByte(it);
    ttl         = readInt16(it);
    bitLength   = readByte(it);
    
    byte byteLength = (bitLength + 7) / 8;
                
    copy (it, it + byteLength, aux);
    source = HypercubeAddress(aux, bitLength);
    it += byteLength;

    copy (it, it + byteLength, aux);
    dest = HypercubeAddress(aux, bitLength);
    it += byteLength;

    protocol = TransportType(readByte(it));

    int dataLength = totalLength - (7 + 2 * byteLength);    
    
    copy (it, it + dataLength, back_inserter(data));
    it += dataLength;
    
    // read all the optional headers until "0" found as header type
    while (*it != 0) {
        byte type = TOptionalHeader::getType(it);
        if (type == RouteHeader::HEADER_TYPE) {
            route.push_back(RouteHeader(it));
        } else {
            throw invalid_argument("Unexpected header type: " + type);
        }        
    }    
    
}


/**
 * @brief Get the source address for the packet.
 *
 * @return the source address for the packet.
 */ 
HypercubeAddress DataPacket::getSourceAddress() const
{
    return source;
}

/**
 * @brief Get the destination address for the packet.
 *
 * @return the destination address for the packet.
 */ 
HypercubeAddress DataPacket::getDestinationAddress() const
{
    return dest;
}

/**
 * @brief Get the transport protocol for the packet.
 *
 * @return the transport protocol  for the packet.
 */ 
TransportType DataPacket::getTransportType() const
{
    return protocol;
}


/**
 * @brief Dump the data packet as raw data to the iterator.
 *
 * @param it iterator where raw data is dumped.
 */
void DataPacket::dumpTo(BackItVB it) const
{
    dumpInt16To(totalLength, it);
    dumpByteTo(flags, it);
    dumpInt16To(ttl, it);
    dumpByteTo(source.getBitLength(), it);
    source.dumpTo(it);
    dest.dumpTo(it);
    dumpByteTo(protocol.getType(), it);
    copy(data.begin(), data.end(), it);

    for (int i=0; i < route.size(); i++)
        route[i].dumpTo(it);
        
    dumpByteTo(0, it); // header type indicating end of optional headers.      
}

/**
 * @brief Get the Time To Live of the packet.
 *
 * @return the Time To Live of the packet.
 */
int16 DataPacket::getTTL() const
{
    return ttl;
}

/**
 * @brief Set the Time To Live of the packet.
 *
 * @param ttl the Time To Live of the packet.
 */
void DataPacket::setTTL(int16 ttl)
{
    this->ttl = ttl;
}


/**
 * @brief Get whether this packet is being routed as returned.
 *
 * @return whether this packet is being routed as returned.
 */
bool DataPacket::isReturned() const
{
    return getFlag(0);
}

/**
 * @brief Set whether this packet is being routed as returned.
 *
 * @param returned whether this packet is being routed as returned.
 */
void DataPacket::setReturned(bool returned)
{
    setFlag(0, returned);
}

/**
 * @brief Add a Route Header, used for tracing route.
 *
 * @param rh route header to add
 */
void DataPacket::addRouteHeader(const RouteHeader &rh)
{
    route.push_back(rh);
}

/**
 * @brief Get the route that the packet followed.
 *
 * @return a vector of RouteHeader with the route that the packet followed.
 */
vector<RouteHeader> DataPacket::getRoute() const
{
     return route;   
}

/**
 * @brief Get whether this packet is used to trace route.
 *
 * @return whether this packet is used to trace route.
 */
bool DataPacket::isTraceRoute() const
{
    return getFlag(1);
}

/**
 * @brief Set whether this packet is used to trace route.
 *
 * @param traceRoute whether this packet is used to trace route.
 */
void DataPacket::setTraceRoute(bool traceRoute)
{
    setFlag(1, traceRoute);
}

/**
 * @brief Get whether this packet is sending Rendez Vous information
 *
 * @return whether this packet is sending Rendez Vous information
 */
bool DataPacket::isRendezVous() const
{
    return getFlag(2);
}

/**
 * @brief Set whether this packet is sending Rendez Vous information
 *
 * @param rendezVous whether this packet is sending Rendez Vous information
 */
void DataPacket::setRendezVous(bool rendezVous)
{
    setFlag(2, rendezVous);
}

/**
 * @brief Get whether this packet is undoing a loop.
 *
 * @return whether this packet is undoing a loop.
 */
bool DataPacket::isUnloop() const
{
    return getFlag(3);
}

/**
 * @brief Set whether this packet is undoing a loop.
 *
 * @param unloop whether this packet is undoing a loop.
 */
void DataPacket::setUnloop(bool unloop)
{
    setFlag(3, unloop);
}

/**
 * @brief Helper method to set a flag.
 *
 * @param n the flag to set.
 * @param value value of the flag.
 */
void DataPacket::setFlag(int n, bool value)
{
    if (value) flags |= 0x80 >> n;
    else flags &= 0xFF - (0x80 >> n);        
}

/**
 * @brief Helper method to retrieve a flag.
 *
 * @param n the flag to retrieve.
 * @return the value of flag n.
 */
bool DataPacket::getFlag(int n) const
{
    return (flags & (0x80 >> n)) != 0;    
}

/**
 * @brief Get information about the data packet.
 * 
 * @param options not used.
 */
QueryResult *DataPacket::query(const vector<string> *options) const
{
    QueryResult *qr = new QueryResult("Packet", "DataPacket");
    qr->insert("source", source.toString());
    qr->insert("destination", dest.toString());
    qr->insert("totalLength", toStr(totalLength));
    qr->insert("TTL", toStr(ttl));
    if (isRendezVous())  qr->insert("rendezVous", "true");
    if (isReturned())  qr->insert("returned", "true");
    if (isUnloop())  qr->insert("unloop", "true");
    return qr;
}

}
}
}
