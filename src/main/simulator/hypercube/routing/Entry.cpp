#include "Entry.h"
#include "DataPacket.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace simulator::hypercube::dataUnit;

/**
 * @brief Create an Entry for the routing table.
 *
 * @param dest festination for this entry.
 * @param nextHop Next Hop
 * @param distance distance to the destination if known or MAX_TTL if not
 * @param mapping map of the neighbours that are already visited.
 */
Entry::Entry(const HypercubeAddress &dest, const HypercubeAddress &nextHop, int distance, NeighbourMapping *mapping) :
    dest(dest), nextHop(nextHop), distance(distance), visitedBitmap(mapping)
{
}


/**
 * @brief Create an Entry for the routing table.
 *
 * @param distance distance to the destination if known or MAX_TTL if not
 * @param mapping map of the neighbours that are already visited.
 */
Entry::Entry(const HypercubeAddress &dest,  NeighbourMapping *mapping) :
    dest(dest), distance(DataPacket::MAX_TTL), visitedBitmap(mapping)
{
}

/**
 * @brief Virtual destructor
 */
Entry::~Entry()
{
}

/**
 * @brief Set the destination for this entry.
 *
 * @param dest destination for this entry.
 */
void Entry::setDestination(const HypercubeAddress &dest)
{
    this->dest = dest;
}

/**
 * @brief Set the next hop for this entry.
 *
 * @param nextHop next hop for this entry.
 */
void Entry::setNextHop(const HypercubeAddress &nextHop)
{
    this->nextHop = nextHop;
}


/**
 * @brief Set the distance for this entry.
 *
 * @param distance distance for this entry.
 */
void Entry::setDistance(int distance)
{
    this->distance = distance;
}

/**
 * @brief Get the destination for this entry.
 * 
 * @return the destination for this entry.
 */
HypercubeAddress Entry::getDestination() const
{
    return dest;
}

/**
 * @brief Get the next hop for this entry.
 * 
 * @return the next hop for this entry.
 */
HypercubeAddress Entry::getNextHop() const
{
    return nextHop;
}

/**
 * @brief Get the distance for this entry.
 * 
 * @return the distance for this entry.
 */
int Entry::getDistance() const
{
    return distance;
}

/**
 * @brief Get the map of visited neighbours.
 *
 * @return the map of visited neighbours.
 */
VisitedBitmap *Entry::getVisitedBitmap()
{
    return &visitedBitmap;
}

/**
 * @brief Compares two entries. They're considered equal if they have the same
 * destination, next hop and distance.
 *
 * @param entry entry to compare with.
 * @return whether entry is es equal to this entry.
 */
bool Entry::operator==(const Entry &entry) const
{
    return this->dest == entry.dest &&
           this->nextHop == entry.nextHop &&
            this->distance == entry.distance; 
}

/**
 * @brief Get information about the entry.
 *
 * @param option options for this query, not used.
 */ 
QueryResult *Entry::query(const vector<string> *options) const
{
    QueryResult *entry =  new QueryResult("Entry");
    entry->insert("Destination", dest.toString());
    entry->insert("NextHop", nextHop.toString());        
    entry->insert("Distance", toStr(distance));        
    entry->insert("VisitedBitmap", visitedBitmap.toString());        
    return entry;    
}


}}}
