#include "HCPacket.h"
#include "ProtocolIDs.h"
#include "RouteHeader.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {

using namespace std;
using namespace simulator::layer;


/**
 * @brief Empty constructor.
 */
RouteHeader::RouteHeader() : address(HypercubeAddress(0)), TOptionalHeader(HEADER_TYPE)
{
}


/**
 * @brief Create a route header.
 *
 * @param address hypercube address of the hop.
 * @param isReturned whether it went to the hop as returned.
 */
RouteHeader::RouteHeader(const HypercubeAddress &address, bool isReturned)
    : address(address), TOptionalHeader(HEADER_TYPE), time(time)
{
    setFlag(0, isReturned);    
    length = 11 + (address.getBitLength() + 7) / 8;
}


/**
 * @brief Get whether it went to the hop as returned.
 *
 * @return whether it went to the hop as returned.
 */
bool RouteHeader::isReturned() const
{
    return getFlag(0);
}


/**
 * @brief Create a Route Header header from raw bytes.
 *
 * @param it iterator pointing to the start of raw data.  The iterator
 * will advance to the end of the header.
 */
RouteHeader::RouteHeader(ConstItVB &it) : TOptionalHeader(HEADER_TYPE), address(0)
{
    byte aux[256];
    
    typeAndFlags = readByte(it);
    length = readByte(it);
            
    byte addrBitLength = readByte(it);
    byte addrLength = (addrBitLength + 7) / 8;    

    copy (it, it + addrLength, aux);
    address = HypercubeAddress(aux, addrBitLength);
    it += addrLength;         
}

/**
 * @brief Get the route address.
 *
 * @return the proposed address.
 */
HypercubeAddress RouteHeader::getAddress() const
{
    return address;
}


/**
 * @brief Dump the header to raw data.
 *
 * @param it the iterator where the data will be dumped.
 */
void RouteHeader::dumpTo(BackItVB it) const
{
    dumpByteTo(typeAndFlags, it);
    dumpByteTo(length, it);

    dumpByteTo(address.getBitLength(), it);
    address.dumpTo(it);    
}


/**
 * @brief Get information about this header.
 *
 * @param option options for the query, not used.
 */
QueryResult *RouteHeader::query(const vector<string> *options) const
{
    QueryResult *qr = new QueryResult("RouteHeader");
    qr->insert("route", address.toString());
    if (isReturned())  {
        qr->insert("returned", "true");
    }
    return qr;
}




}
}
}
