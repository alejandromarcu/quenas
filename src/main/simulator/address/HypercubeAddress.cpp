#include <stdexcept>
#include "HypercubeAddress.h"
#include "HypercubeMaskAddress.h"

namespace simulator {
    namespace address {

/** 
 * @brief Creates an Hypercube Address from a byte array.
 *
 * @param addr the byte array containing the address
 * @param bitLength the length in bits of the address
 */
HypercubeAddress::HypercubeAddress(byte *addr, int bitLength) : 
    TNetworkAddress(addr, (bitLength + 7) / 8), bitLength(bitLength)
{
}

/** 
 * @brief Creates an empty Hypercube Address (length 0).
 */
HypercubeAddress::HypercubeAddress() : bitLength(0)
{
}

/** 
 * @brief Creates an Hypercube Address of all zeros.
 *
 * @param bitLength the length in bits of the address
 */
HypercubeAddress::HypercubeAddress(int bitLength) : bitLength(bitLength)
{
    address = vector<byte>((bitLength + 7) / 8);
}

/**
 * @brief Create an Hypercube Address from its string representation, that must
 * consist of 1's and 0's.
 *
 * @param str the string representation of the address to build.
 */
HypercubeAddress::HypercubeAddress(const string &str) : bitLength(str.length())
{
    address = vector<byte>((bitLength + 7) / 8);
    for(int i = 0; i < str.length(); i++) {
        if (str[i] == '0') setBit(i, 0);
        else if (str[i] == '1') setBit(i, 1);
        else throw invalid_argument("Invalid address: " + str);
    }    
}


/** 
 * @brief Get the length in bits of the address.
 *
 * @return the length in bits of the address.
 */
int HypercubeAddress::getBitLength() const {
    return bitLength;
}

/** 
 * @brief Get a bit of the addess.
 *
 * @param n the bit to retrieve
 * @return 1 if the bit is set, 0 if it is not set.
 */
int HypercubeAddress::getBit(int n) const 
{
    if (n < 0 || n >= bitLength) throw invalid_argument("HypercubeAddress::getBit: n is out of range");
    
    return (address[n >> 3] & (128 >> ( n % 8))) > 0? 1 : 0;
}

/** 
 * @brief Sets a bit in the address.
 *
 * @param n the bit to set
 * @param value 1 to set the bit (or ommit it), 0 to clear the bit.
 */
void HypercubeAddress::setBit(int n, int value)
{
    if (n < 0 || n >= bitLength) throw invalid_argument("HypercubeAddress::setBit: n is out of range");

    if (value == 0) {
        address[n >> 3] &= 255 - (128 >> ( n % 8));
    } else {
        address[n >> 3] |= 128 >> ( n % 8);
    }
}

/** 
 * @brief flip the value of a bit.
 *
 * @param n the bit to flip
 */
void HypercubeAddress::flipBit(int n) 
{
   if (n < 0 || n >= bitLength)  throw invalid_argument("HypercubeAddress::flipBit: n is out of range " + toStr(n));

    address[n >> 3] ^= 128 >> ( n % 8);
}

/** 
 * @brief Get the string representation of the address.
 *
 * Hypercube Addresses are represented in binary, i.e. 1010110
 * @return the string representation of the address
 */
string HypercubeAddress::toString() const
{
    string s = "";
    for(int i = 0; i < bitLength; i++)
        s += getBit(i)? "1" : "0";
    return s;
}


/**
 * @brief Calculate the distance between addresses, ie how many bits are different.
 *
 * @param addr the addres to measure the distance to.
 * @return the number of bits that are different.
 */
int HypercubeAddress::distance(const HypercubeAddress &addr) const {
    int count = 0;
    for (int i = 0; i < bitLength; i++) {
        count += addr.getBit(i) != getBit(i);
    }
    return count;      
}

// end namespaces
}
}
