#include <vector>
#include <iterator>


#include "RendezVousPacket.h"
#include "common.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "StateMachines.h"
#include "HypercubeNode.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;

const byte RendezVousRegister::TYPE = 1;
const byte RendezVousDeregister::TYPE = 2;
const byte RendezVousAddressSolve::TYPE = 3;
const byte RendezVousAddressLookup::TYPE = 4;
const byte RendezVousLookupTable::TYPE = 5;
const byte RendezVousLookupTableReceived::TYPE = 6;

int16 RendezVousLookupTable::globalId = 0;  

//-------------------------------------------------------------------------
//------------------------< TRendezVousPacket >----------------------------
//------------------------------------------------------------------------- 
/**
 * @brief Create a base Rendez Vous Packet.
 * 
 * @param typeAndFlags type of packet and its flags.
 */
TRendezVousPacket::TRendezVousPacket(byte typeAndFlags) : typeAndFlags(typeAndFlags)
{
}

/**
 * @brief Create a packet from raw data,.
 *
 * @param data raw data to process.
 * @param an instance of a TRendezVousPacket subclass.
 */
TRendezVousPacket *TRendezVousPacket::create(const VB &data)
{
    byte type = data[0] & 0x1F;
    TRendezVousPacket *packet;
    switch (type) {
        case RendezVousRegister::TYPE  : packet = new RendezVousRegister(data); break;
        case RendezVousDeregister::TYPE  : packet = new RendezVousDeregister(data); break;
        case RendezVousAddressSolve::TYPE  : packet = new RendezVousAddressSolve(data); break;
        case RendezVousAddressLookup::TYPE  : packet = new RendezVousAddressLookup(data); break;
        case RendezVousLookupTable::TYPE  : packet = new RendezVousLookupTable(data); break;
        case RendezVousLookupTableReceived::TYPE  : packet = new RendezVousLookupTableReceived(data); break;
        default: throw invalid_argument("Invalid rendez vous packet type: " + type);            
    }
    return packet;
}

/**
 * @brief Helper method to read an hypercube address from raw data.
 * 
 * @param it iterator where the address will be read.
 * @return an hypercube address.
 */
HypercubeAddress TRendezVousPacket::readHypercubeAddress(ConstItVB &it) const
{
    byte aux[256];
    byte addrBitLength = readByte(it);
    byte addrLength = (addrBitLength + 7) / 8;
    
    copy (it, it + addrLength, aux);
    HypercubeAddress addr(aux, addrBitLength);
    it += addrLength;      

    return addr;
}

/**
 * @brief Get the packet data as a vector<byte>
 * 
 * @return the packet data as a vector<byte>
 */
VB TRendezVousPacket::getData() const 
{
    VB data;
    dumpTo(back_inserter(data));
    return data;
}

/**
 * @brief Get the specified flag (from 0 to 2 inclusive).
 *
 * @param n the flag to retrieve, from 0 to 2 inclusive
 * @return the n flag (n from 0 to 2 inclusive)
 */
bool TRendezVousPacket::getFlag(int n) const
{
    return (typeAndFlags & (0x80 >> n)) != 0;
}

/**
 * @brief Set the specified flag (from 0 to 2 inclusive).
 *
 * @param n flag to set (from 0 to 2 inclusive).
 * @param value value for the flag.
 */
void TRendezVousPacket::setFlag(int n, bool value)
{
    if (value) typeAndFlags |= 0x80 >> n;
    else typeAndFlags &= 0xFF - (0x80 >> n);    
}

/**
 * @brief Get the type of packet.
 *
 * @return the type of packet.  
 */
byte TRendezVousPacket::getType() const
{
    return typeAndFlags & 0x1F;
}

//-------------------------------------------------------------------------
//------------------------< RendezVousRegister >---------------------------
//------------------------------------------------------------------------- 

/**
 * @brief Create a packet from raw data.
 *
 * @param data data to read.
 */
RendezVousRegister::RendezVousRegister(const VB &data) : TRendezVousPacket(TYPE), universalAddress("")
{
    ConstItVB it = data.begin();
    byte aux[256];

    typeAndFlags = readByte(it);
    primaryAddress = readHypercubeAddress(it);       
    universalAddress = UniversalAddress(readString(it));
    
}

/**
 * @brief Create a RV packet.
 *
 * @param primaryAddress hypercube address of the node to register.
 * @param universalAddress universal address of the node to register.
 */ 
RendezVousRegister::RendezVousRegister(const HypercubeAddress &primaryAddress, const UniversalAddress &universalAddress)
    : TRendezVousPacket(TYPE), primaryAddress(primaryAddress), universalAddress(universalAddress)
{
}

/**
 * @brief Get the primary address of the registering node.
 *
 * @return the primary address of the registering node.
 */
HypercubeAddress RendezVousRegister::getPrimaryAddress() const
{
    return primaryAddress;
}

/**
 * @brief Get the universal address of the registering node.
 *
 * @return the universal address of the registering node.
 */
UniversalAddress RendezVousRegister::getUniversalAddress () const
{
    return universalAddress;
}

/**
 * @brief Dump the RV packet as raw data to the iterator.
 *
 * @param it iterator where data is dumped.
 */
void RendezVousRegister::dumpTo(BackItVB it) const 
{
    dumpByteTo(typeAndFlags, it);
    dumpByteTo(primaryAddress.getBitLength(), it);
    primaryAddress.dumpTo(it);
    dumpStringTo(universalAddress.toString(), it);
}

//-------------------------------------------------------------------------
//-----------------------< RendezVousDeregister >--------------------------
//------------------------------------------------------------------------- 
/**
 * @brief Create a packet from raw data.
 *
 * @param data data to read.
 */
RendezVousDeregister::RendezVousDeregister(const VB &data) : TRendezVousPacket(TYPE), universalAddress("")
{
    ConstItVB it = data.begin();
    byte aux[256];

    typeAndFlags = readByte(it);
    primaryAddress = readHypercubeAddress(it);       
    universalAddress = UniversalAddress(readString(it));
    
}

/**
 * @brief Create a RV packet.
 *
 * @param primaryAddress hypercube address of the node to deregister.
 * @param universalAddress universal address of the node to deregister.
 */ 
RendezVousDeregister::RendezVousDeregister(const HypercubeAddress &primaryAddress, const UniversalAddress &universalAddress)
    : TRendezVousPacket(TYPE), primaryAddress(primaryAddress), universalAddress(universalAddress)
{
}

/**
 * @brief Get the primary address of the deregistering node.
 *
 * @return the primary address of the deregistering node.
 */
HypercubeAddress RendezVousDeregister::getPrimaryAddress() const
{
    return primaryAddress;
}

/**
 * @brief Get the universal address of the deregistering node.
 *
 * @return the universal address of the deregistering node.
 */
UniversalAddress RendezVousDeregister::getUniversalAddress () const
{
    return universalAddress;
}

/**
 * @brief Dump the RV packet as raw data to the iterator.
 *
 * @param it iterator where data is dumped.
 */
void RendezVousDeregister::dumpTo(BackItVB it) const 
{
    dumpByteTo(typeAndFlags, it);
    dumpByteTo(primaryAddress.getBitLength(), it);
    primaryAddress.dumpTo(it);
    dumpStringTo(universalAddress.toString(), it);
}


//-------------------------------------------------------------------------
//----------------------< RendezVousAddressSolve >------------------------
//------------------------------------------------------------------------- 
/**
 * @brief Create a packet from raw data.
 *
 * @param data data to read.
 */
RendezVousAddressSolve::RendezVousAddressSolve(const VB &data) : TRendezVousPacket(TYPE), universalAddress("")
{
    ConstItVB it = data.begin();
    byte aux[256];

    typeAndFlags = readByte(it);
    universalAddress = UniversalAddress(readString(it));    
}

/**
 * @brief Create a RendezVousAddressSolve packet.
 *
 * @param universalAddress universal address to solve
 */ 
RendezVousAddressSolve::RendezVousAddressSolve(const UniversalAddress &universalAddress)
    : TRendezVousPacket(TYPE), universalAddress(universalAddress)
{
}

/**
 * @brief Get the universal address to solve.
 *
 * @return the universal address to solve.
 */
UniversalAddress RendezVousAddressSolve::getUniversalAddress () const
{
    return universalAddress;
}

/**
 * @brief Dump the RV packet as raw data to the iterator.
 *
 * @param it iterator where data is dumped.
 */
void RendezVousAddressSolve::dumpTo(BackItVB it) const 
{
    dumpByteTo(typeAndFlags, it);
    dumpStringTo(universalAddress.toString(), it);
}


//-------------------------------------------------------------------------
//---------------------< RendezVousAddressLookup >-------------------------
//------------------------------------------------------------------------- 
/**
 * @brief Create a packet from raw data.
 *
 * @param data data to read.
 */
RendezVousAddressLookup::RendezVousAddressLookup(const VB &data) : TRendezVousPacket(TYPE), universalAddress("")
{
    ConstItVB it = data.begin();
    byte aux[256];

    typeAndFlags = readByte(it);
    primaryAddress = readHypercubeAddress(it);       
    universalAddress = UniversalAddress(readString(it));
    
}

/**
 * @brief Create a RendezVousAddressLookup packet.
 *
 * @param primaryAddress hypercube address of the looked up node.
 * @param universalAddress universal address of the looked up node.
 * @param solved whether the address could be solved or not.
 */ 
RendezVousAddressLookup::RendezVousAddressLookup(const HypercubeAddress &primaryAddress, 
    const UniversalAddress &universalAddress, bool solved)
    : TRendezVousPacket(TYPE), primaryAddress(primaryAddress), universalAddress(universalAddress)
{
    setFlag(0, solved);
}

/**
 * @brief Get the solved primaryy address.
 *
 * @return the solved primary address.
 */
HypercubeAddress RendezVousAddressLookup::getPrimaryAddress() const
{
    return primaryAddress;
}

/**
 * @brief Get the universal address looked up.
 *
 * @return the universal address looked up.
 */
UniversalAddress RendezVousAddressLookup::getUniversalAddress () const
{
    return universalAddress;
}

/**
 * @brief Return whether the addres could be solved.
 *
 * @return whether the addres could be solved.
 */
bool RendezVousAddressLookup::isSolved() const 
{
    return getFlag(0);
}

/**
 * @brief Dump the RV packet as raw data to the iterator.
 *
 * @param it iterator where data is dumped.
 */
void RendezVousAddressLookup::dumpTo(BackItVB it) const 
{
    dumpByteTo(typeAndFlags, it);
    dumpByteTo(primaryAddress.getBitLength(), it);
    primaryAddress.dumpTo(it);
    dumpStringTo(universalAddress.toString(), it);
}

//-------------------------------------------------------------------------
//-----------------------< RendezVousLookupTable >-------------------------
//------------------------------------------------------------------------- 
/**
 * @brief Create a packet from raw data.
 *
 * @param data data to read.
 */
RendezVousLookupTable::RendezVousLookupTable(const VB &data) : TRendezVousPacket(TYPE)
{
    ConstItVB it = data.begin();
    byte aux[256];
        
    typeAndFlags = readByte(it);
    id = readInt16(it);
    int n = readInt16(it);

    byte addrBitLength = readByte(it);
    byte addrLength = (addrBitLength + 7) / 8;
    

    for (int i = 0; i < n; i++) {
        UniversalAddress u(readString(it));
        
        copy (it, it + addrLength, aux);
        HypercubeAddress p(aux, addrBitLength);
        it += addrLength;      
        
        table.push_back(make_pair(p, u));
    }
}

/**
 * @brief Create a RendezVousLookupTable packet.
 * It automatically generates an id that will be used to acknowledge it.
 * Data from the lookupo table should be added with add method.
 */
RendezVousLookupTable::RendezVousLookupTable() : id(globalId++), TRendezVousPacket(TYPE)
{
}

/**
 * @brief get the table contained in this packet.
 *
 * @return the table contained in this packet.
 */
const RendezVousLookupTable::TTABLE &RendezVousLookupTable::getTable() const
{
    return table;
}


/**
 * @brief Dump the RV packet as raw data to the iterator.
 *
 * @param it iterator where data is dumped.
 */
void RendezVousLookupTable::dumpTo(BackItVB it) const
{
    dumpByteTo(typeAndFlags, it);
    dumpInt16To(id, it);
    dumpInt16To(table.size(), it);
    
    byte bitLength = 0;
    if (table.size() > 0) bitLength = table[0].first.getBitLength();
    
    dumpByteTo(bitLength, it);
    
    for (int i = 0; i < table.size(); i++) {
        dumpStringTo(table[i].second.toString(), it);        
        
        table[i].first.dumpTo(it);
    }        
}

/**
 * @brief Add an entry to the table in the packet.
 *
 * @param primaryAddress primary address in the new entry.
 * @param universalAddress universal address in the new entry.
 */
void RendezVousLookupTable::add(const HypercubeAddress &primaryAddress, const UniversalAddress &universalAddress)
{
    table.push_back(make_pair(primaryAddress, universalAddress));
}

/**
 * @brief Get the identifier for this packet, used to acknowledge it.
 *
 * @return the identifier for this packet, used to acknowledge it.
 */
int16 RendezVousLookupTable::getId() const
{
    return id;
}

//-------------------------------------------------------------------------
//-------------------< RendezVousLookupTableReceived >---------------------
//------------------------------------------------------------------------- 
/**
 * @brief Create a packet from raw data.
 *
 * @param data data to read.
 */
RendezVousLookupTableReceived::RendezVousLookupTableReceived(const VB &data) : TRendezVousPacket(TYPE)
{
    ConstItVB it = data.begin();
        
    typeAndFlags = readByte(it);
    id = readInt16(it);
}

/**
 * @brief Create a RendezVousLookupTableReceived packet.
 *
 * @param id id that is being acknowledged.
 */
RendezVousLookupTableReceived::RendezVousLookupTableReceived(int16 id) : id(id), TRendezVousPacket(TYPE)
{
}

/**
 * @brief Dump the RV packet as raw data to the iterator.
 *
 * @param it iterator where data is dumped.
 */
void RendezVousLookupTableReceived::dumpTo(BackItVB it) const
{
    dumpByteTo(typeAndFlags, it);
    dumpInt16To(id, it);
}

/**
 * @brief Get the acknowledged id.
 *
 * @return the acknowledged id.
 */
int16 RendezVousLookupTableReceived::getId() const
{
    return id;
}

}
}

