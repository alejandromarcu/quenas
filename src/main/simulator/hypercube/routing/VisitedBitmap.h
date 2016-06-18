#ifndef _VISITEDBITMAP_H_
#define _VISITEDBITMAP_H_

#include <vector>
#include "HypercubeAddress.h"
#include "NeighbourMapping.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace std;
using namespace simulator::address;

/**
 * @brief Contains a bitmap of which neighbours are already visited.
 */
class VisitedBitmap {
    public:
        VisitedBitmap(NeighbourMapping *mapping);
        
        void setVisited(int n);
        bool isVisited(int n) const;

        int visitedCount() const;
        
        void clear();
        string toString() const;
        
        NeighbourMapping *getMapping();
    private:
        /// Neighbours already visited.
        vector<bool> visited;

        /// Mapping to the actual neighbour addresses
        NeighbourMapping *mapping;
        
};


}}}

#endif
