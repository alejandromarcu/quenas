#include <vector>
#include "HypercubeAddress.h"
#include "VisitedBitmap.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace std;
using namespace simulator::address;


/**
 * @brief Create a Visited Bitmap.
 *
 * @param mapping mapping associated with this bitmap.
 */
VisitedBitmap::VisitedBitmap(NeighbourMapping *mapping) : mapping(mapping)
{
    visited = vector<bool>(mapping->size(), false);
}

/**
 * @brief Set a neighbour as visited.
 *
 * @param n neighbour to set as visited.
 */
void VisitedBitmap::setVisited(int n)
{
    // if n is outside the array, fill it with falses
    while (n >= visited.size()) visited.push_back(false);     
    
    visited[n] = true;
}

/**
 * @brief Get whether a neighbour is visted.
 *
 * @param n neighbour to check.
 * @return whether neighbour n is visted.
 */
bool VisitedBitmap::isVisited(int n) const
{
    if (n < 0 || n >= visited.size())  return false;

    return visited[n];    
}


/**
 * @brief Get how many neighbours are already visited.
 *
 * @return how many neighbours are already visited.
 */
int VisitedBitmap::visitedCount() const
{
    int n=0;
    for (int i = 0; i < visited.size(); i++) {
        if (visited[i]) n++;
    }
    
    return n;

}

/**
 * @brief Get the mapping associated with this bitmap.
 *
 * @return the mapping associated with this bitmap.
 */
NeighbourMapping *VisitedBitmap::getMapping()
{
    return mapping;
}

/**
 * @brief Erase the bitmap, setting all the nodes as unvisited.
 */
void VisitedBitmap::clear() 
{
    visited = vector<bool>(mapping->size(), false);
}

/**
 * @brief Get a string representation for the bitmap.
 *
 * @return a string representation for the bitmap (0 not visited, 1 visited)
 */
string VisitedBitmap::toString() const
{
    string s;
    for (int i = 0; i < visited.size(); i++) {
       s += visited[i]? "1" : "0";
    }

    return s;
}


}}}
