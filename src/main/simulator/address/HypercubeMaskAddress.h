#ifndef _HYPERCUBEMASKADDRESS_H_
#define _HYPERCUBEMASKADDRESS_H_
#include "HypercubeAddress.h"
#include "common.h"

namespace simulator {
    namespace address {

/** 
 * @brief Represents an Hypercube Address and its mask.
 *
 * The hypercube address has variable length in bits; the mask is an
 * integer repredenting how many bits from left to right are part of this space.
 * Its string representation is its binary value + "/" + mask, for example "1010/3"
 */
class HypercubeMaskAddress : public HypercubeAddress {
    private:
        /// length of the mask in bits
        int mask;
        
    public:
        HypercubeMaskAddress(const HypercubeAddress &address, int mask);
        HypercubeMaskAddress(const string &address, int mask);
        HypercubeMaskAddress(byte *addr, int bitLength, int mask);
        
        void setMask(int mask);
        int getMask() const;
        virtual string toString() const;
        bool contains(const HypercubeAddress &addr) const;
        bool contains(const HypercubeMaskAddress &addr) const;

        virtual int distanceWithMask(const HypercubeAddress &addr) const;
        virtual int distanceWithMask(const HypercubeMaskAddress &addr) const;
        
};

// end namespaces
}
}
#endif
