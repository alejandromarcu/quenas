#include "Neighbour.h"

namespace simulator {
    namespace hypercube {


/**
 * @brief Create a new Neighbour.
 *
 * @param paddr the primary address of the neighbour.
 * @param uaddr the physical address of the neighbour.         
 */
Neighbour::Neighbour(const HypercubeMaskAddress &paddr, const MACAddress &phaddr) 
    : type(NOT_CONNECTED), primaryAddress(paddr), physicalAddress(phaddr), active(true),proposedSecondaryAddress(false)
{
}
                   
/**
 * @brief Set the primary address of the neighbour.
 *
 * @param addr addres of the neighbour.
 */    
void Neighbour::setPrimaryAddress(const HypercubeMaskAddress &addr) {
    primaryAddress = addr;
}
        
/**
 * @brief Get the primary address of the neighbour.
 *
 * @return the primary address of the neighbour.
 */
HypercubeMaskAddress Neighbour::getPrimaryAddress() const {
    return primaryAddress;
}

/**
 * @brief Set the physical address of the neighbour.
 *
 * @param addr physical of the neighbour.
 */    
void Neighbour::setPhysicalAddress(const MACAddress &addr) {
    physicalAddress = addr;
}

/**
 * @brief Get the physical address of the neighbour.
 *
 * @return the physical address of the neighbour.
 */
MACAddress Neighbour::getPhysicalAddress() const {
    return physicalAddress;
}

/**
 * @brief Set whether a secondary address was proposed to the neighbour.
 *
 * @param proposed whether a secondary address was proposed to the neighbour.
 */
void Neighbour::setProposedSecondaryAddress(bool proposed) {
    proposedSecondaryAddress = proposed;
}


/**
 * @brief Get whether a secondary address was proposed to the neighbour.
 *
 * @return whether a secondary address was proposed to the neighbour.
 */
bool Neighbour::hasProposedSecondaryAddress() const {
    
    return proposedSecondaryAddress;
}

/**
 * @brief Set the type of neighbour.
 *
 * @param type the type of neighbour.
 */
void Neighbour::setType(Neighbour::NeighbourType type) {
    this->type = type;
}

/**
 * @brief Get the type of neighbour.
 *
 * @return the type of neighbour.
 */
Neighbour::NeighbourType Neighbour::getType() const {
    return type;
}

/**
 * @brief Get a name for the type of neighbour.
 *
 * @return a name for the type of neighbour.
 */
string Neighbour::getTypeName() const {
    switch(type) {
     case PARENT      : return "parent";
     case CHILD       : return "child";
     case ADJACENT    : return "adjacent";
     case NOT_CONNECTED : return "not connected";
     case DISSAPEARED : return "dissapeared";
     case DISCONNECTED : return "disconnected";
    }
    return "UNKNOWN";
}

/**
 * @brief Get whether the neighbour is marked as active.
 *
 * @return whether the neighbour is marked as active.
 */
bool Neighbour::isActive() const {
    return active;
}

/**
 * @brief Set whether the neighbour is marked as active.
 *
 * @param active whether the neighbour is marked as active.
 */
void Neighbour::setActive(bool active) {
    this->active = active;
}

/**
 * @brief Get the time when the neighbour last sent an Heard Bit.
 *
 * @return the time when the neighbour last sent an Heard Bit.
 */
Time Neighbour::getLastSeen() const {
    return lastSeen;
}

/**
 * @brief Set the time when the neighbour last sent an Heard Bit.
 *
 * @param t the time when the neighbour last sent an Heard Bit.
 */
void Neighbour::setLastSeen(Time t) {
    lastSeen = t;
}


}}
