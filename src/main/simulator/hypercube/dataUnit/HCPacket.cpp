#include "HCPacket.h"
#include "ProtocolIDs.h"
#include "RouteHeader.h"
#include "DataPacket.h"
#include "MultiCommandRunner.h"
#include "CommandQueryResult.h"
#include "Exceptions.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {

using namespace std;
using namespace simulator::layer;

const byte PARPacket::TYPE = 1;
const byte PAPPacket::TYPE = 2;
const byte PANPacket::TYPE = 3;
const byte PANCPacket::TYPE = 4;
const byte DiscPacket::TYPE = 5;
const byte HBPacket::TYPE = 6;
const byte SAPPacket::TYPE = 7;
const byte SANPacket::TYPE = 8;

const byte AdditionalAddress::HEADER_TYPE = 1;
const byte RouteHeader::HEADER_TYPE = 4;


const EthernetType TControlPacket::ETHERNET_TYPE = EthernetType(1000); 
const EthernetType DataPacket::ETHERNET_TYPE = EthernetType(1001); 


//-------------------------------------------------------------------------
//--------------------------< AdditionalAddress >--------------------------
//-------------------------------------------------------------------------
/**
 * @brief Create an Empty Additional Address header.
 */
AdditionalAddress::AdditionalAddress() : address(HypercubeAddress(0)), TOptionalHeader(HEADER_TYPE)
{
}

/**
 * @brief Create an Additional Address header.
 *
 * @param address hypercube address.
 * @param mask mask for the address.
 * @param connectionCount number of connections that the node has
 */
AdditionalAddress::AdditionalAddress(const HypercubeAddress &address, byte mask, byte connectionCount)
    : address(address), mask(mask), connectionCount(connectionCount), TOptionalHeader(HEADER_TYPE)
    
{
    setFlag(0, true);
    setFlag(1, true);
    length = 5 + (address.getBitLength() + 7) / 8;
}

/**
 * @brief Create an Additional Address header.
 *
 * @param address hypercube address.
 * @param mask mask for the address.
 */
AdditionalAddress::AdditionalAddress(const HypercubeAddress &address, byte mask)
    : address(address), mask(mask), connectionCount(0), TOptionalHeader(HEADER_TYPE)
    
{
    setFlag(0, true);
    setFlag(1, false);
    length = 4 + (address.getBitLength() + 7) / 8;
}


/**
 * @brief Create an Additional Address header, not carrying any mask.
 *
 * @param address hypercube address.
 */
AdditionalAddress::AdditionalAddress(const HypercubeAddress &address)
    : address(address), mask(0), connectionCount(0), TOptionalHeader(HEADER_TYPE)
    
{
    setFlag(0, false);
    setFlag(1, false);
    length = 3 + (address.getBitLength() + 7) / 8;
}


/**
 * @brief Set the mask length.
 *
 * @param mask mask length.
 */
void AdditionalAddress::setMask(byte mask)
{
    setFlag(0, true);
    this->mask = mask;
}

/**
 * @brief Set the Connection Count value
 *
 * @param connectionCount the Connection Count value
 */
void AdditionalAddress::setConnectionCount(byte connectionCount)
{
   setFlag(1, true);
   this->connectionCount = connectionCount;    
}


/**
 * @brief Get whether this header carries an additional address.
 *
 * @return whether this header carries an additional address.
 */
bool AdditionalAddress::hasMask() const
{
    return getFlag(0);
}

/**
 * @brief Get whether this header carries a connection count value.
 *
 * @return whether this header carries a connection count value.
 */
bool AdditionalAddress::hasConnectionCount() const
{
    return getFlag(1);
}


/**
 * @brief Create an Address Proposal header from raw bytes.
 *
 * @param it iterator pointing to the start of raw data.  The iterator
 * will advance to the end of the header.
 */
AdditionalAddress::AdditionalAddress(ConstItVB &it) : TOptionalHeader(HEADER_TYPE), address(0)
{
    byte aux[256];
    
    typeAndFlags = readByte(it);
    length = readByte(it);
            
    byte addrBitLength = readByte(it);
    byte addrLength = (addrBitLength + 7) / 8;    

    copy (it, it + addrLength, aux);
    address = HypercubeAddress(aux, addrBitLength);
    it += addrLength;      
    
    if (hasMask()) {
        mask = readByte(it);
    }
    
    if (hasConnectionCount()) {
        connectionCount = readByte(it);            
    }
}

/**
 * @brief Get the proposed address with his mask.
 *
 * @return the proposed address with his mask.
 */
HypercubeMaskAddress AdditionalAddress::getMaskAddress() const
{
    return HypercubeMaskAddress(address, mask);
}


/**
 * @brief Get the proposed address.
 *
 * @return the proposed address.
 */
HypercubeAddress AdditionalAddress::getAddress() const
{
    return address;
}

/**
 * @brief Get the mask for the proposed address.
 *
 * @return the mask for the proposed address.
 */
byte AdditionalAddress::getMask() const
{
    return mask;
}

/**
 * @brief Get the number of connections that the node has.
 *
 * @return the number of connections that the node has.
 */
byte AdditionalAddress::getConnectionCount() const
{
    return connectionCount;
}

/**
 * @brief Dump the header to raw data.
 *
 * @param it the iterator where the data will be dumped.
 */
void AdditionalAddress::dumpTo(BackItVB it) const
{
    dumpByteTo(typeAndFlags, it);
    dumpByteTo(length, it);

    dumpByteTo(address.getBitLength(), it);
    address.dumpTo(it);    

    if (hasMask()) {
        dumpByteTo(mask, it);
    }

    if (hasConnectionCount()) {
        dumpByteTo(connectionCount, it);        
    }
}


/**
 * @brief Get information about this header.
 *
 * @param options options for the query, not used.
 * @return a QueryResult with information about this header.
 */
QueryResult *AdditionalAddress::query(const vector<string> *options) const
{
    QueryResult *qr = new QueryResult("AddressProposal");
    qr->insert("proposedAddress", address.toString());
    qr->insert("mask", toStr((int)mask));
    return qr;
}


//----------------------------------------------------------------------
//--------------------------< TControlPacket >--------------------------
//----------------------------------------------------------------------
/**
 * @brief Create a control packet from its fields.
 *
 * @param type type of packet.
 * @param physicalAddress Physical Address of the sending node
 * @param primaryAddress Primary Address of the sending node.
 */
TControlPacket::TControlPacket(byte type, const MACAddress &physicalAddress, 
        const HypercubeMaskAddress &primaryAddress) 
        : typeAndFlags(type), physicalAddress(physicalAddress), primaryAddress(primaryAddress)
{
    totalLength = 11 + (primaryAddress.getBitLength() + 7) / 8;
}

TControlPacket::TControlPacket() : primaryAddress("", 0)
{
}

/**
 * @brief Fill the packet fields from a frame's payload.
 *
 * @frame frame to read its payload.
 */

void TControlPacket::read(const TFrame &frame) 
{
    ConstItVB it = frame.getPayload().begin();
    byte aux[256];
    
    typeAndFlags = readByte(it);
    totalLength  = readByte(it);
            
    copy (it, it + 6, aux);
    physicalAddress = MACAddress(aux);
    it += 6;

    byte addrBitLength = readByte(it);
    byte addrLength = (addrBitLength + 7) / 8;
    
    copy (it, it + addrLength, aux);
    it += addrLength;      
    
    byte mask = readByte(it);
    primaryAddress = HypercubeMaskAddress(aux, addrBitLength, mask);

    while (*it != 0) {
        byte type = TOptionalHeader::getType(it);
        if (type == AdditionalAddress::HEADER_TYPE) {
            addOptionalHeader(AdditionalAddress(it));
        } else {
            throw invalid_argument("unknown optional header type: " + type);
        }        
    }
        
}

/**
 * @brief Virtual destructor.  Does nothing.
 */
TControlPacket::~TControlPacket()
{
}

/**
 * @brief Get the type of packet.
 *
 * @return the type of packet.  
 */
byte TControlPacket::getType() const
{
    return typeAndFlags & 0x1F;
}

/**
 * @brief Get the specified flag (from 0 to 2 inclusive).
 *
 * @param n the flag to retrieve, from 0 to 2 inclusive
 * @return the n flag (n from 0 to 2 inclusive)
 */
bool TControlPacket::getFlag(int n) const
{
    return (typeAndFlags & (0x80 >> n)) != 0;
}

/**
 * @brief Get the Physical Address of the sending node
 *
 * @return the Physical Address of the sending node.
 */
MACAddress TControlPacket::getPhysicalAddress() const
{
    return physicalAddress;
}

/**
 * @brief Get the Primary Address of the sending node.
 * 
 * @return Primary Address of the sending node.
 */
HypercubeMaskAddress TControlPacket::getPrimaryAddress() const
{
    return primaryAddress;
}

/**
 * @brief Dump the packet to raw data.
 *
 * @param it the iterator where the data will be dumped.
 */
void TControlPacket::dumpTo(BackItVB it) const
{
    dumpByteTo(typeAndFlags, it);
    dumpByteTo(totalLength, it);
    physicalAddress.dumpTo(it);
    dumpByteTo(primaryAddress.getBitLength(), it);    
    primaryAddress.dumpTo(it);
    dumpByteTo(primaryAddress.getMask(), it);

    dumpOptionalHeadersTo(it);

    dumpByteTo(0, it); // header type indicating end of optional headers.
}

/**
 * @brief Create a packet from row data.
 * An object of the appropiate packet type will be returned.
 *
 * @return an instance of a packet class.
 */
TControlPacket *TControlPacket::create(const TFrame &frame)
{
    byte type = frame.getPayload()[0] & 0x1F;
    TControlPacket *packet;
    switch (type) {
        case PARPacket::TYPE  : packet = new PARPacket(); break;
        case PAPPacket::TYPE  : packet = new PAPPacket(); break;
        case PANPacket::TYPE  : packet = new PANPacket(); break;
        case PANCPacket::TYPE : packet = new PANCPacket(); break;
        case DiscPacket::TYPE : packet = new DiscPacket(); break;
        case HBPacket::TYPE   : packet = new HBPacket(); break;
        case SAPPacket::TYPE  : packet = new SAPPacket(); break;
        case SANPacket::TYPE  : packet = new SANPacket();break;
        default: throw invalid_argument("Invalid control packet type: " + type);            
    }
    packet->read(frame);
    return packet;
}

/**
 * @brief Set the specified flag (from 0 to 2 inclusive).
 *
 * @param n flag to set (from 0 to 2 inclusive).
 * @param value value for the flag.
 */
void TControlPacket::setFlag(int n, bool value)
{
    if (value) typeAndFlags |= 0x80 >> n;
    else typeAndFlags &= 0xFF - (0x80 >> n);    
}

/**
 * @brief Called when an AdditionalAddress wants to be added. If it is not overriden,
 * the default behaviour is to not accept them by throwing an exception.
 *
 * @param header header to add.
 */
void TControlPacket::addOptionalHeader(AdditionalAddress header) {
    throw logic_error("Unexpected optional header in packet type " + toStr((int)getType()));
}

/**
 * @brief Dump the optional headers to an iterator as raw data.
 * Subclasses using optional headers MUST override this method.
 *
 * @param it iterator where raw data is writen.
 */
void TControlPacket::dumpOptionalHeadersTo(BackItVB it) const
{
}

/**
 * @brief Get information about additional fields in the packet.
 * Subclasses having more fields than this class must override this method.
 *
 * @param qr QueryResult where additional information must be added.
 */
void TControlPacket::queryAdditionalFields(QueryResult *qr) const
{
}

/**
 * @brief Run a command.
 *
 * @brief function function to run.
 */
TCommandResult *TControlPacket::runCommand(const Function &function)
{
   if (function.getName() == "query")  
   {
        return new CommandQueryResult(query()); 
   }

    throw command_error("TControlPacket: Bad function: " + function.toString());     
}

/**
 * @brief Get information about this packet.
 *
 * @param options options for the query, not used.
 * @return a QueryResult with information about this packet
 */
QueryResult *TControlPacket::query(const vector<string> *options) const   
{
    QueryResult *qr = new QueryResult("Packet", getName());
    qr->insert("physicalAddress", physicalAddress.toString());
    qr->insert("primaryAddress", primaryAddress.toString());
    queryAdditionalFields(qr);

    return qr;    
}

byte TControlPacket::getTotalLength() const
{
    return totalLength;
}

//-----------------------------------------------------------------------
//-----------------------------< PARPacket >-----------------------------
//-----------------------------------------------------------------------

/**
 * @brief Empty Constructor.
 */
PARPacket::PARPacket() 
{ 
}

/**
 * @brief Create a PAR packet.
 *
 * @param physicalAddress address of the node requesting the primary Address.
 */
PARPacket::PARPacket(const MACAddress &physicalAddress) 
    : TControlPacket(TYPE, physicalAddress, HypercubeMaskAddress("",0)) 
{
}

/**
 * @brief Get this packet name.
 *
 * @eturn "PAR"
 */
string PARPacket::getName() const 
{ 
    return "PAR"; 
}



//-----------------------------------------------------------------------
//-----------------------------< PAPPacket >-----------------------------
//-----------------------------------------------------------------------

/**
 * @brief Empty Constructor.
 */
PAPPacket::PAPPacket() 
{ 
}

/**
 * @brief Create a PAP packet with address space exhausted
 *
 * @param physicalAddress physical address of the node proposing the primary address.
 * @param primaryAddress primary address of the node proposing the primary address.
 */          
PAPPacket::PAPPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress)
          : TControlPacket(TYPE, physicalAddress, primaryAddress)

{
    setFlag(0, true);
}

/**
 * @brief Create a PAP packet proposing a primary address an optionally reconnection addresses.
 *
 * @param physicalAddress physical address of the node proposing the primary address.
 * @param primaryAddress primary address of the node proposing the primary address.
 * @param proposed proposed primary address
 * @param reconnect addresses proposed for reconnection
 */
PAPPacket::PAPPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress, 
                  const AdditionalAddress &proposed,  const vector<AdditionalAddress> &reconnect)                
        : TControlPacket(TYPE, physicalAddress, primaryAddress), proposed(proposed), reconnect(reconnect)
{
    setFlag(0, false);
    totalLength += proposed.getLength();

    for (int i = 0; i < reconnect.size(); i++) {
        totalLength += reconnect[i].getLength();
    }
}


/**
 * @brief Return wheter address is exhausted on the node.
 *
 * @return wheter address is exhausted on the node.
 */
bool PAPPacket::isAddressExhausted() const
{
    return getFlag(0);
}

/**
 * @brief Get the optional header for the proposed address.
 *
 * @return the optional header for the proposed address.
 */
AdditionalAddress PAPPacket::getProposed() const
{
    return proposed;
}

/**
 * @brief Get the reconnect address optional headers.
 *
 * @return the reconnect address optional headers.
 */
vector<AdditionalAddress> PAPPacket::getReconnect() const
{
    return reconnect;
}

/**
 * @brief Get this packet name.
 *
 * @eturn "PAP"
 */
string PAPPacket::getName() const 
{ 
    return "PAP"; 
}
        
/**
 * @brief Get information about the additional fields in this packet.
 *
 * @param qr QueryResult where information is added.
 */        
void PAPPacket::queryAdditionalFields(QueryResult *qr) const
{
    if (isAddressExhausted()) {
        qr->insert("adressExhausted", "true");
    } else {
       qr->insert("", proposed.query());
    }
        
    if (reconnect.size() > 0) {
        QueryResult *r = new QueryResult("reconnectionAddress");

        for (int i = 0; i < reconnect.size(); i++)
            r->insert("reconnectionAddress", reconnect[i].query());
        
       qr->insert("", r);
    }
}


/**
 * @brief Add an AdditionalAddress header.
 *
 * @param header header to add.
 */
void PAPPacket::addOptionalHeader(AdditionalAddress header) 
{
    if (proposed.getAddress().getBitLength() == 0) {
        proposed = header;        
    } else {
        reconnect.push_back(header);
    }
}

/**
 * @brief Dump the optional headers to the iterator as raw data.
 *
 * @param it iterator where optional headers are dumped.
 */
void PAPPacket::dumpOptionalHeadersTo(BackItVB it) const 
{
    if (!isAddressExhausted()) {
        proposed.dumpTo(it);
    }
    for (int i = 0; i < reconnect.size(); i++)
        reconnect[i].dumpTo(it);
}

        
//-----------------------------------------------------------------------
//-----------------------------< PANPacket >-----------------------------
//-----------------------------------------------------------------------
/**
 * @brief Empty Constructor.
 */
PANPacket::PANPacket() 
{
}

/**
 * @brief Create a PAN packet.
 *
 * @param physicalAddress physical address of the node notifying the primary address acquired.
 * @param primaryAddress primary address that the node acquired.
 */          
PANPacket::PANPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress) 
    : TControlPacket(TYPE, physicalAddress, primaryAddress) 
{
}
        
/**
 * @brief Get this packet name.
 *
 * @eturn "PAN"
 */
string PANPacket::getName() const 
{ 
    return "PAN"; 
}

//-----------------------------------------------------------------------
//-----------------------------< PANCPacket >----------------------------
//-----------------------------------------------------------------------
/**
 * @brief Empty Constructor.
 */
PANCPacket::PANCPacket() 
{
}

/**
 * @brief Create a PANC packet.
 *
 * @param physicalAddress physical address of the node that gave the primary address being confirmed.
 * @param primaryAddress primary aaddress of the node that gave the primary address being confirmed.
 */          
PANCPacket::PANCPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress) 
    : TControlPacket(TYPE, physicalAddress, primaryAddress) 
{    
}
        
/**
 * @brief Get this packet name.
 *
 * @eturn "PANC"
 */
string PANCPacket::getName() const 
{ 
    return "PANC"; 
}

//-----------------------------------------------------------------------
//-----------------------------< DiscPacket >----------------------------
//-----------------------------------------------------------------------
/**
 * @brief Empty Constructor.
 */
DiscPacket::DiscPacket() 
{
}

/**
 * @brief Create a Disc packet.
 *
 * @param physicalAddress physical address of the node that is disconnecting.
 * @param primaryAddress primary address of the node that is disconnecting.
 * @param hasChild whether the node that is disconnecting has children.
 */
DiscPacket::DiscPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress, bool hasChild) 
            : TControlPacket(TYPE, physicalAddress, primaryAddress)
{
    setFlag(0, hasChild);    
}
    
/**
 * @brief Get whether the node has children.  
 * 
 * @return whether the node has children.  
 */
bool DiscPacket::hasChild() const 
{ 
    return getFlag(0); 
}

/**
 * @brief Get this packet name.
 *
 * @eturn "DISC"
 */
string DiscPacket::getName() const 
{ 
    return "DISC"; 
}


//-----------------------------------------------------------------------
//------------------------------< HBPacket >-----------------------------
//-----------------------------------------------------------------------
/**
 * @brief Empty Constructor.
 */
HBPacket::HBPacket() 
{
}

/**
 * @brief Create a HB packet.
 *
 * @param physicalAddress physical address of the node sending the Heard Bit.
 * @param primaryAddress primary aaddress of the node sending the Heard Bit.
 */          
HBPacket::HBPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress) 
    : TControlPacket(TYPE, physicalAddress, primaryAddress) 
{
}
        
/**
 * @brief Get this packet name.
 *
 * @eturn "HB"
 */
string HBPacket::getName() const 
{ 
    return "HB"; 
}


//-----------------------------------------------------------------------
//-----------------------------< SAPPacket >-----------------------------
//-----------------------------------------------------------------------
/**
 * @brief Empty Constructor.
 */
SAPPacket::SAPPacket() 
{
}

/**
 * @brief Create a SAP packet.
 *
 * @param physicalAddress physical address of the node proposing a secondary address.
 * @param primaryAddress primary aaddress of the node proposing a secondary address.
 * @param proposed proposed secondary address.
 */          
SAPPacket::SAPPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress, 
    const AdditionalAddress &proposed) : TControlPacket(TYPE, physicalAddress, primaryAddress), proposed(proposed)  
{
    totalLength += proposed.getLength();    
}


/**
 * @brief Get the proposed secondary address.
 *
 * @return the proposed secondary address.
 */
AdditionalAddress SAPPacket::getProposed() const 
{     
    return proposed;
}

/**
 * @brief Get this packet name.
 *
 * @eturn "SAP"
 */
string SAPPacket::getName() const 
{ 
    return "SAP"; 
}


/**
 * @brief Add an AdditionalAddress header.
 *
 * @param header header to add.
 */
void SAPPacket::addOptionalHeader(AdditionalAddress header) {
    proposed = header;
}

/**
 * @brief Dump the optional headers to the iterator as raw data.
 *
 * @param it iterator where optional headers are dumped.
 */
void SAPPacket::dumpOptionalHeadersTo(BackItVB it) const {
    proposed.dumpTo(it);
}

//-----------------------------------------------------------------------
//-----------------------------< SANPacket >-----------------------------
//-----------------------------------------------------------------------

/**
 * @brief Empty Constructor.
 */
SANPacket::SANPacket() 
{
}

/**
 * @brief Create a SAN packet.
 *
 * @param physicalAddress physical address of the node accepting the secondary address.
 * @param primaryAddress primary aaddress of the node accepting the secondary address.
 * @param proposed accepted proposed secondary address.
 */          
SANPacket::SANPacket(const MACAddress &physicalAddress, const HypercubeMaskAddress &primaryAddress,
            bool accepted, AdditionalAddress proposed)
    : TControlPacket(TYPE, physicalAddress, primaryAddress), proposed(proposed)
{
    setFlag(0, accepted);
    totalLength += proposed.getLength();        
}

/**
 * @brief Get the proposed secondary address.
 *
 * @return the proposed secondary address.
 */
AdditionalAddress SANPacket::getProposed() const 
{     
    return proposed;
}

/**
 * @brief Get whether the secondary addess was accepted.
 *
 * @return whether the secondary addess was accepted.
 */
bool SANPacket::isAccepted() const 
{ 
    return getFlag(0); 
}

/**
 * @brief Get this packet name.
 *
 * @eturn "SAN"
 */
string SANPacket::getName() const 
{ 
    return "SAN"; 
}

/**
 * @brief Get information about the additional fields in this packet.
 *
 * @param qr QueryResult where information is added.
 */        
 void SANPacket::queryAdditionalFields(QueryResult *qr) const
{
    qr->insert("isAccepted", isAccepted()? "true" : "false");            
}


/**
 * @brief Add an AdditionalAddress header.
 *
 * @param header header to add.
 */
void SANPacket::addOptionalHeader(AdditionalAddress header) 
{
    proposed = header;
}

/**
 * @brief Dump the optional headers to the iterator as raw data.
 *
 * @param it iterator where optional headers are dumped.
 */
void SANPacket::dumpOptionalHeadersTo(BackItVB it) const 
{
    proposed.dumpTo(it);
}
        



}
}
}
