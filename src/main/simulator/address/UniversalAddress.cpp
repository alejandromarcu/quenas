#include <stdexcept>
#include "UniversalAddress.h"

namespace simulator {
    namespace address {

/** 
 * @brief Builds an Universal address using 6 bytes from addr
 *
 * @param addr pointer to 6 bytes representing the address
 */
UniversalAddress::UniversalAddress(byte *addr) : TApplicationAddress(addr, 6) 
{
}

/** 
 * @brief Builds an Universal Address represented by a string
 *
 * @param str string representing the universal address.
 */
UniversalAddress::UniversalAddress(const string &str) 
    : TApplicationAddress((byte *) str.c_str(), str.length()) 
{
}

/** 
 * @brief Return the address.
 *
 * @return the address.
 */
string UniversalAddress::toString() const {
    char aux[256];
    for (int i=0; i < address.size(); i++) {
        aux[i] =address[i]; 
    }
    aux[address.size()] = 0;
    string str(aux);
    
    return  str;            
}

/**
 * @brief Hash the Universal Address and makes it fit into a MAC address.
 *
 * @return a MAC address based on the Universal Address.
 */
MACAddress UniversalAddress::hashToMAC() const {
    long long hash = 0;
    for (int i=0; i < address.size(); i++) {
        hash = (hash * 31 + address[i]);
    }            
    return MACAddress((byte *) &hash);
}

/**
 * @brief Hash the Universal Address generating a Hypercube address from it.
 *
 * @return a Hypercube address based on the Universal Address.
 */
HypercubeAddress UniversalAddress::hashToHypercube(int bitLength) const
{

    long long hc[8];
    hc[0] = 0;
    for (int i=0; i < address.size(); i++) {
        hc[0] = hc[0] * 31 + address[i];
    }
    for (int i = 1; i < 8; i++) {
        hc[i] = (hc[i-1] + 17) * 31;
    }

  return HypercubeAddress((byte *) hc,bitLength);
}

// end namespaces
}
}
