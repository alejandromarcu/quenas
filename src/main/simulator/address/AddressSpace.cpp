#include "AddressSpace.h"
#include "HypercubeMaskAddress.h"
#include "common.h"

namespace simulator {
    namespace address {

/**
 * @brief Add an address to the space.
 *
 * @param addr address to added
 * @return true if the space was modified by the addition, false if not.
 */
bool AddressSpace::add(const HypercubeMaskAddress &addr)
{
    // if the address is already represented in the space, nothing needs to be done.
    if (contains(addr)) return false;
    
    // if the address is the whole space, consider as a special case, because
    // it can't be searched for the "complement".
    if (addr.getMask() == 0) {
        base.clear();
        base.insert(addr);
        return true;
    }    
    
    // the complement is the address that if found, can be summarized with the new
    // address and have a mask 1 bit smaller.
    HypercubeMaskAddress complement = addr;
    complement.flipBit(addr.getMask() - 1);

    base_it it = base.find(complement);
    
    if (it != base.end()) {
        // if found a complementary address:
            
        // erase the complementary address
        base.erase(it);
        
        // create in z the address that summarizes both addresses.
        HypercubeMaskAddress z = addr;
        z.setBit(z.getMask() - 1, 0);
        z.setMask(z.getMask() - 1); 
        
        // add z recursively, so that it can be summarized again if needed.
        add(z);                
    } else {
        // the complementary address was not found.
        
        // check if there are addresses contained by the new address and erase them.
        for (base_it it = base.begin(); it != base.end(); it++) {         
            if (addr.contains(*it)) base.erase(it);
        }
        base.insert(addr);

    }

    return true;
}

/**
 * @brief Return whether the address space contains a mask address.
 *
 * @param addr address to test
 * @return whether the address space contains a mask address.
 */
bool AddressSpace::contains(const HypercubeMaskAddress &addr) const
{
    for (base_const_it it = base.begin(); it != base.end(); it++) {
        if (it->getMask() > addr.getMask()) return false;
        
        if (it->contains(addr)) return true;
    }
    
    return false;
}

/**
 * @brief Return a minimal set of the addresses that compose the space.
 *
 * @return a minimal set of the addresses that compose the space.
 */
vector<HypercubeMaskAddress> AddressSpace::getBase() const
{
    vector<HypercubeMaskAddress> result;

    for (base_it it = base.begin(); it != base.end(); it++) {
        result.push_back(*it);        
    }
    
    return result;
}

// end namespaces
}
}
