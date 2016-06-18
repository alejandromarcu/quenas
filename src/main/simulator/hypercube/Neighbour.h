#ifndef _NEIGHBOUR_H_
#define _NEIGHBOUR_H_

#include <vector>
#include <iterator>

#include "common.h"
#include "HCPacket.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "Frame.h"
#include "Simulator.h"
#include "HypercubeMaskAddress.h"
#include "MACAddress.h"
#include "TRoutingAlgorithm.h"
#include "DataPacket.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::hypercube::routing;
using namespace simulator::message;
using namespace simulator::event;
using namespace simulator;
using namespace simulator::address;
      
class TStateMachine;
class HypercubeControlLayer;
class PAPSM;
class HBLSM;
class MainSM;


    
/**
 * @brief Holds the data for a neighbour of the node.
 */    
class Neighbour {
    public:
        /// Types of neighbours
        typedef enum NeighbourType { PARENT, CHILD, ADJACENT, NOT_CONNECTED, DISSAPEARED, DISCONNECTED };
        
        Neighbour(const HypercubeMaskAddress &paddr, const MACAddress &phaddr);
        
        void setPrimaryAddress(const HypercubeMaskAddress &addr);
        HypercubeMaskAddress getPrimaryAddress() const;

        void setPhysicalAddress(const MACAddress &addr);
        MACAddress getPhysicalAddress() const;
        
        void setProposedSecondaryAddress(bool proposed = true);
        bool hasProposedSecondaryAddress() const;

        void setType(NeighbourType type);
        NeighbourType getType() const;

        string getTypeName() const;

        bool isActive() const;
        void setActive(bool active);

        Time getLastSeen() const;
        void setLastSeen(Time t);
        
    private:
        /// Primary address of the neighbour.
        HypercubeMaskAddress primaryAddress;
        
        /// Physical address of the neighbour.        
        MACAddress physicalAddress;
        
        // Indicates whether a secondary address has been already proposed to that neighbour.
        bool proposedSecondaryAddress;
        
        /// Type of neighbour.
        NeighbourType type;
        
        /// Last time that the neighbour sent an HB
        Time lastSeen;
        
        /// Whether the neighbour has been marked as active
        bool active;
};
  
        
}
}

#endif
