#ifndef _NEIGHBOURMAPPING_H_
#define _NEIGHBOURMAPPING_H_

#include <vector>
#include "HypercubeMaskAddress.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace std;
using namespace simulator::address;

/**
 * @brief Class for mapping between the bitmap of visited nodes and their addresses,
 * and containing whether each node is available or not.
 */
class NeighbourMapping {
    public:
        void setParent(const HypercubeMaskAddress &parent);
        void add(const HypercubeMaskAddress &neigh);
        HypercubeMaskAddress getAddress(int n) const;
        int findIndex(const HypercubeAddress &addr) const;

        void changeMask(const HypercubeMaskAddress &addr);
        
        bool isAvailable(int n) const;        
        void setAvailable(int n, bool isAvailable);
        
        int size() const;
    private:
        /// whether each neighbour is available or not.
        vector<bool> available;
        
        /// Neighbour addresses
        vector<HypercubeMaskAddress> addresses;
        
};


}}}

#endif
