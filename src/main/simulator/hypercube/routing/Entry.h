#ifndef _ENTRY_H_
#define _ENTRY_H_

#include "Notification.h"
#include "HypercubeAddress.h"
#include "NeighbourMapping.h"
#include "VisitedBitmap.h"

namespace simulator {
    namespace hypercube {
        namespace routing {
            
using namespace simulator::address;            

/**
 * @brief Represents an entry in the Routing table.
 */
class Entry :  public TQueryable {
    public:
        Entry(const HypercubeAddress &dest, const HypercubeAddress &nextHop, int distance, NeighbourMapping *mapping);
        Entry(const HypercubeAddress &dest, NeighbourMapping *mapping);
        virtual ~Entry();
        
        void setDestination(const HypercubeAddress &dest);
        HypercubeAddress getDestination() const;

        void setNextHop(const HypercubeAddress &nextHop);
        HypercubeAddress getNextHop() const;

        void setDistance(int distance);
        int getDistance() const;
        
        VisitedBitmap *getVisitedBitmap();
    
        bool operator==(const Entry &entry) const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;    
    private:
        /// Destination for this entry.
        HypercubeAddress dest;

        /// Next hop 
        HypercubeAddress nextHop;
        
        /// Distance to the destination if known or MAX_TTL if not
        int distance;
        
        /// Map of the neighbours that are already visited.
        VisitedBitmap visitedBitmap;
       
};

}}}
#endif
