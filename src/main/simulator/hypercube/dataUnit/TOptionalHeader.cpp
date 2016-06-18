#include "TOptionalHeader.h"
#include "HCPacket.h"
#include "RouteHeader.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {


/**
 * @brief Create a new optional header of the specified type.
 *
 * @param type type of optional header.
 */
TOptionalHeader::TOptionalHeader(byte type) : typeAndFlags(type)
{
}

/**
 * @brief Virtual destructor. Does nothing.
 */
TOptionalHeader::~TOptionalHeader()
{
}

/**
 * @brief Get the header type.
 * 
 * @return the header type.
 */
byte TOptionalHeader::getHeaderType() const
{
    return typeAndFlags & 0x1F;
}

/**
 * @brief Get the specified flag (from 0 to 2 inclusive).
 *
 * @param n the flag to retrieve, from 0 to 2 inclusive
 * @return the n flag (n from 0 to 2 inclusive)
 */
bool TOptionalHeader::getFlag(int n) const
{
    return (typeAndFlags & (0x80 >> n)) != 0;
}

/**
 * @brief Create an optional header from an iterator pointing to the start of
 * a header.
 *
 * @param it iterator pointing to the start of a header.
 * @return an optional header of the appropiate type.
 */
TOptionalHeader* TOptionalHeader::create(ConstItVB &it)
{
    byte type = getType(it);
    switch (type) { 
        case 1: return new AdditionalAddress(it);
        case 4: return new RouteHeader(it);
        default: throw invalid_argument("Invalid header type: " + type);           
    }    
}

/**
 * @brief Get the header type.
 *
 * @return the header type.
 */
byte TOptionalHeader::getType(ConstItVB &it) 
{
    return *it & 0x1F;
}


/**
 * @brief Set the specified flag (from 0 to 2 inclusive).
 *
 * @param n flag to set (from 0 to 2 inclusive).
 * @param value value for the flag.
 */
void TOptionalHeader::setFlag(int n, bool value)
{  
    if (value) typeAndFlags |= 0x80 >> n;
    else typeAndFlags &= 0xFF - (0x80 >> n);
}

/**
 * @brief Get the length for this optional header.
 *
 * @return the length for this optional header.
 */
byte TOptionalHeader::getLength() const 
{
    return length;
}

}}}
