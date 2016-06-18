#ifndef _HYPERCUBEADDRESS_H_
#define _HYPERCUBEADDRESS_H_

#include "TNetworkAddress.h"
#include "common.h"

namespace simulator {
    namespace address {

class HypercubeMaskAddress;

/** 
 * @brief Represents an Hypercube Address.
 *
 * The hypercube address has variable length in bits.
 * Its string representation is its binary value.
 */
class HypercubeAddress : public TNetworkAddress {
    private:
        // Lenght in bits of the address
        int bitLength;
        
    public:
        HypercubeAddress();
        HypercubeAddress(byte *addr, int bitLength);
        HypercubeAddress(int bitLength);
        HypercubeAddress(const string &str);
        
        int getBitLength() const;
        int getBit(int n) const;
        void setBit(int n, int value = 1);
        void flipBit(int n);
        virtual string toString() const;
        int distance(const HypercubeAddress &addr) const;              
};

// end namespaces
}
}

#endif
