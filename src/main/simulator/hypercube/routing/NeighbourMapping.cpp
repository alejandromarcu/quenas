#include "NeighbourMapping.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace simulator::address;

/**
 * @brief Set who is the parent of the node.  It is stored in the first bit.
 *
 * @param parent address of the parent.
 */
void NeighbourMapping::setParent(const HypercubeMaskAddress &parent) {    
    if (addresses.size() == 0) {
        addresses.push_back(parent);
        available.push_back (true);
    } else { 
        addresses[0] = parent;
        available[0] = true;
    }
}
   
/**
 * @brief Add a neigbour to the list.  It is added at the end.
 *
 * @param neigh neighbour to add.
 */        
void NeighbourMapping::add(const HypercubeMaskAddress &neigh) {    
    available.push_back(true);
    addresses.push_back(neigh);
}

/**
 * @brief Get whether a neigbour is available.
 *
 * @param n the neighbour to check.
 * @return whether the neigbour n is available.
 */
bool NeighbourMapping::isAvailable(int n) const {
    if (n < 0 || n >= available.size()) 
        throw invalid_argument("NeighbourMapping::isAvailable - n out of range: " + toStr(n));
        
    return available[n];
}

/**
 * @brief Set whether a neigbour is available.
 *
 * @param n the neighbour to set
 * @param isAvailable whether the neigbour n is available.
 */
void NeighbourMapping::setAvailable(int n, bool isAvailable) {
    if (n < 0 || n >= available.size()) 
        throw invalid_argument("NeighbourMapping::setAvailable - n out of range: " + toStr(n));

    available[n] = isAvailable;
}

/**
 * @brief Get the address of a neighbour.
 *
 * @param n the index of the neigbour to get its address.
 * @return the address of neighbour n.
 */
HypercubeMaskAddress NeighbourMapping::getAddress(int n) const {
    if (n < 0 || n >= available.size()) 
        throw invalid_argument("NeighbourMapping::getAddress - n out of range: " + toStr(n));

    return addresses[n];
}

/**
 * @brief Get the size of this mapping.
 *
 * @return the size of this mapping.
 */
int NeighbourMapping::size() const
{
    return available.size();
}

/**
 * @brief Find the index for an address.
 *
 * @param addr the address to find its index.
 * @return the index of the neigbour with that address or -1 if not found.
 */
int NeighbourMapping::findIndex(const HypercubeAddress &addr) const
{
    for (int i = 0; i < addresses.size(); i++) {
        if (addr == ((HypercubeAddress) addresses[i])) return i;
    }
    return -1;
}

/**
 * @brief Change the mask of a neighbour.
 *
 * @param the address with the mask that has changed.
 */
void NeighbourMapping::changeMask(const HypercubeMaskAddress &addr) 
{
    int idx = findIndex(addr);
    if (idx >= 0) {
        addresses[idx] = addr;
    }
}

}}}
