#ifndef _TADDRESSSPACE_H_
#define _TADDRESSSPACE_H_

#include <vector>
#include <iterator>
#include <set>

#include "common.h"
#include "HypercubeMaskAddress.h"

namespace simulator {
    namespace address {

using namespace std;

/**
 * Comparator for two HypercubeMaskAddresses.
 */
struct comparator
{
    /**
     * @brief Compares two HypercubeMaskAddress.
     * 
     * The one with smaller mask is "bigger", and in case of a tie,
     * their string representation is compeared.
     * 
     * @param x address to compare
     * @param y address to compare
     * @return true iff x is "bigger" than y
     */
    bool operator()(const HypercubeMaskAddress &x, const HypercubeMaskAddress &y) const
    {
        if (x.getMask() < y.getMask()) return true;
        if (x.getMask() > y.getMask()) return false;        
        return x.toString() < y.toString();
    }    
};

/**
 * @brief Represents an address space.
 * An address space is a set of mask addresses.  This class also sumarizes the
 * set to store the minimum equivalent representation.
 */
class AddressSpace {    
    public:        
        bool add(const HypercubeMaskAddress &addr);
        bool contains(const HypercubeMaskAddress &addr) const;
        
        vector<HypercubeMaskAddress> getBase() const;
        
    private:
        typedef set<HypercubeMaskAddress, comparator> base_type;
        typedef base_type::iterator base_it;
        typedef base_type::const_iterator base_const_it;
        
        // Set of mask addresses
        base_type base;
        
};



// end namespaces
}
}

#endif
