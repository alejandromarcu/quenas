#include "HypercubeMaskAddress.h"
#include "common.h"

namespace simulator {
    namespace address {

/**
 * @brief Create an HypercubeMaskAddress from an Hypercube Address and a mask
 *
 * @param address the address
 * @param mask mask length in bits
 */
HypercubeMaskAddress::HypercubeMaskAddress(const HypercubeAddress &address, int mask)
    : HypercubeAddress(address), mask(mask)
{
}

/**
 * @brief Create an HypercubeMaskAddress from its Hypercube Address string
 * representationd and a mask.
 *
 * @param address string representation of the hypercube address
 * @param mask mask length in bits
 */
HypercubeMaskAddress::HypercubeMaskAddress(const string &address, int mask)
    : HypercubeAddress(address), mask(mask)
{
}

/**
 * @brief Create an HypercubeMaskAddress from a byte array.
 *
 * @param addr byte array where the address is stored
 * @param bitLenght length of the address in bits
 * @param mask mask length in bits
 */
HypercubeMaskAddress::HypercubeMaskAddress(byte *addr, int bitLength, int mask)
    : HypercubeAddress(addr, bitLength), mask(mask) {
};

/**
 * @brief Set the mask length
 *
 * @param mask the mask length to set
 */
void HypercubeMaskAddress::setMask(int mask) {
    if (mask < 0 || mask > getBitLength()) 
        throw invalid_argument("HypercubeMaskAddress::setMask: mask is out of range " + toStr(mask));
        
    this->mask = mask;
}

/**
 * @brief Returns the mask length in bits.
 *
 * @return the mask length in bits.
 */
int HypercubeMaskAddress::getMask() const {
    return mask;
}

/**
 * @brief Returns a string representation in the format address/mask, for
 * example "1010/3"
 *
 * @return a string representation in the format address/mask, for
 * example "1010/3"
 */
string HypercubeMaskAddress::toString() const {
    return HypercubeAddress::toString() + "/" + toStr(mask);
}

/**
 * @brief Determines if the address is contained into the address
 * space covered by this object.
 *
 * @param an Address to be tested for inclusion
 * @return true if addr is contained into this space.
 */
bool HypercubeMaskAddress::contains(const HypercubeAddress &addr) const 
{
    if (addr.getBitLength() != getBitLength()) 
        throw invalid_argument("Addresses are of diferent size");


    // if any of the bits included in the mask of this address is diferent
    // than the corresponding bit in addr, then is not included
    for (int i=0; i < getMask(); i++) {
        if (addr.getBit(i) != getBit(i)) {
            return false;
        }
    }

    // all the bits in mask are equal, so it's included.
    return true;
}

/**
 * @brief Determines if the address space is contained into the address
 * space covered by this object.
 *
 * @param an address space to be tested for inclusion
 * @return true if addr is contained into this space.
 */
bool HypercubeMaskAddress::contains(const HypercubeMaskAddress &addr) const {
    // if the mask is smaller, then addr represents a bigger space
    if (addr.getMask() < getMask()) return false;

    return contains((HypercubeAddress &) addr);
}



int HypercubeMaskAddress::distanceWithMask(const HypercubeAddress &addr) const 
{
    int count = 0;    
    for (int i = 0; i < getMask(); i++) {
        count += addr.getBit(i) != getBit(i);
    }        
    return count;
}

int HypercubeMaskAddress::distanceWithMask(const HypercubeMaskAddress &addr) const 
{
    int count = 0;
    int l = addr.getMask() < getMask()? addr.getMask() : getMask();
    
    for (int i = 0; i < l; i++) {
        count += addr.getBit(i) != getBit(i);
    }        
    return count;
}


// end namespaces
}
}
