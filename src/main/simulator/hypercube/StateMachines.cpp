#include <vector>
#include <iterator>

#include "common.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "StateMachines.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "HypercubeNode.h"
#include "HypercubeNetwork.h"
#include "HypercubeParameters.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;
      
      
//----------------------------------------------------------------------
//------------------------------< TState >------------------------------
//----------------------------------------------------------------------      
/**
 * @brief This method is called when the state machine is entering in the state.
 * This base method does nothing.
 *
 * @param from the previous state.
 * @return NULL to indicate no state change by default. 
 */
TState* TState::onEnter(TState *from)
{
    return NULL;
}

/**
 * @brief This method is called when the state machine is changing state.
 * This base method calls cancelAllTimeouts.
 */ 
void TState::onExit()
{
    cancelAllTimeouts();
}

/**
 * @brief This method is called when a requested time out is over.
 * This base method removes the timeout from the internal list.
 *
 * @param id id of the timeout that is over.
 * @return NULL to indicate no state change by default.
 */
TState* TState::onTimeout(int id)
{
    vector<int>::iterator it = find(timeouts.begin(), timeouts.end(), id);
    if (it != timeouts.end()) timeouts.erase(it);
    return NULL;
}

/**
 * @brief This method is called when a message is received.
 * This base method does nothing.
 *
 * @param message the message received.
 * @return NULL to indicate no state change by default.
 */
TState* TState::onMessageReceived(const TMessage *message)
{
    return NULL;
}

/**
 * @brief This method is called when a packet is received.
 * This base method does nothing.
 *
 * @param packet the packet received.
 * @return NULL to indicate no state change by default.
 */
TState* TState::onPacketReceived(const TControlPacket &packet)
{
    return NULL;   
}

/**
 * @brief Request a timeout. 
 * When the timeout is over, method onTimeout will be called, using the same id
 * that this method returns.
 *
 * @param elapsed amount of time for the timeout.
 * @return the id of the time out.
 */
int TState::addTimeout(Time elapsed)
{
    int id = getStateMachine()->addTimeout(elapsed);
    timeouts.push_back(id);
    return id;
}


/**
 * @brief Cancel all pending timeouts for this state.
 */
void TState::cancelAllTimeouts()
{
    for (int i = 0; i < timeouts.size(); i++) 
        getStateMachine()->cancelTimeout(timeouts[i]);
}

/**
 * @brief Get a pointer to the Hypercube Control Layer of the node.
 *
 * @return a pointer to the Hypercube Control Layer of the node.
 */
HypercubeControlLayer *TState::getHypercubeControlLayer() const
{
    return getStateMachine()->getHypercubeControlLayer();
}

/**
 * @brief Get a pointer to the Hypercube Node.
 *
 * @return a pointer to the Hypercube Node.
 */
HypercubeNode *TState::getNode() const
{
    return static_cast<HypercubeNode *>(getStateMachine()->getHypercubeControlLayer()->getNode());
}

//----------------------------------------------------------------------
//--------------------------< TStateMachine >---------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a state machine.
 *
 * @param node the node owning this state machine.
 */
TStateMachine::TStateMachine(HypercubeControlLayer *hcl) : hcl(hcl), timeoutId(0)
{
}

/**
 * @brief Virtual destructor.  Does nothing.
 */
TStateMachine::~TStateMachine()
{
}    
    
/**
 * @brief This method is called when a timeout is over.
 * The onTimeout method of the current state is called, 
 * and if needed a state change is done.
 * WARNING: the onTimeout method is called on the current state, so if
 * a state has not cancelled all their timeouts when exiting, the timeout
 * may be raised on a different state.
 *
 * @param id id of the raised timeout.
 */
void TStateMachine::onTimeout(int id)
{
    changeState(currentState->onTimeout(id));   

    map<int, TimeoutEvent*>::iterator it = timeouts.find(id);
    if (it != timeouts.end()) {
        timeouts.erase(it);
    }
 
}


/**
 * @brief This method is called when a message is received
 * The onMessageReceived method of the current state is called,
 * and if needed a state change is done.
 *
 * @param message the message received
 */
void TStateMachine::onMessageReceived(const TMessage *message)
{
    changeState(currentState->onMessageReceived(message));
}


/**
 * @brief This method is called when a packet is received
 * The onPacketReceived method of the current state is called, 
 * and if needed a state change is done.
 *
 * @param packet the packet received
 */
void TStateMachine::onPacketReceived(const TControlPacket &packet)
{
    changeState(currentState->onPacketReceived(packet));
}

/**
 * @brief Ask to raise a timeout.
 *
 * @param elapsed elapsed time to raise the time out.
 * @return id of the time out.
 */
int TStateMachine::addTimeout(Time elapsed)
{
    TimeoutEvent *event = new TimeoutEvent(elapsed, this, timeoutId);
    Simulator::getInstance()->addEvent(event, true);

    timeouts[timeoutId] = event;
    return timeoutId++;    
}

/**
 * @brief Cancel a timeout.
 *
 * @param id id of the time out to cancel.
 */
void TStateMachine::cancelTimeout(int id)
{
    map<int, TimeoutEvent*>::iterator it = timeouts.find(id);

    if (it != timeouts.end()) {
        it->second->cancel();
        timeouts.erase(it);
    }
}

/**
 * @brief Get the node owning this state machine.
 *
 * @return the node owning this state machine.
 */
HypercubeNode *TStateMachine::getNode() const
{
    return static_cast<HypercubeNode *>(hcl->getNode());
}

/**
 * @brief Get the Hypercube Control Layer owning this state machine.
 *
 * @return the Hypercube Control Layer owning this state machine.
 */
HypercubeControlLayer *TStateMachine::getHypercubeControlLayer() const
{
    return hcl;
}

/**
 * @brief Change the current state of the state machine.
 * If the new state is null, it does nothing.
 * If it is not null, first onExit is called on the old
 * state, then onEnter is called in the new state.
 *
 * @param newState the new state for this state machine.
 */
void TStateMachine::changeState(TState *newState)
{
    if(newState != NULL) {
        // Exit the current state
        if (currentState != NULL) {
            currentState->onExit();
            Simulator::getInstance()->notify("node.statemachine." + toLower(getName()) + "." + toLower(currentState->getName()) + ".exit", 
                    NULL, getHypercubeControlLayer()->getNode());
        }

        // Enter to the current state
        TState *enterState = newState->onEnter(currentState);     
        Simulator::getInstance()->notify("node.statemachine." + toLower(getName()) + "." + toLower(newState->getName()) + ".enter", 
                    NULL, getHypercubeControlLayer()->getNode());
        
        currentState = newState;       
        
        changeState(enterState);        
    }        
}

      
//----------------------------------------------------------------------
//------------------------------< PAPSM >------------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Primary Address Provider State Machine.
 * 
 * @param hcl pointer to the node HypercubeControlLayer
 */ 
PAPSM::PAPSM(HypercubeControlLayer *hcl) : TStateMachine(hcl) {
    waitPAR = new WaitPAR(this);
    waitPAN = new WaitPAN(this);
    currentState = waitPAR;
}

/**
 * @brief Destructor.  Deletes its states.
 */
PAPSM::~PAPSM()
{
    delete waitPAR;
    delete waitPAN;
}

/**
 * @brief Get the state name
 *
 * @return "PAP"
 */
string PAPSM::getName() const 
{ 
    return "PAP"; 
}
 
 
bool PAPSM::isProposingPrimaryAddress() const
{
    return currentState == waitPAN;
}

//----------------------------------------------------------------------
//---------------------------< PAPSM::State >---------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a State for PAP SM.
 *
 * @param stateMachine PAP SM of the state.
 */
PAPSM::State::State(PAPSM *stateMachine) : stateMachine(stateMachine) 
{    
}  

/**
 * @brief Destroy the state.
 * It cancels all the timeouts.
 */              
PAPSM::State::~State() 
{                   
    cancelAllTimeouts();                
}

/**
 * @brief Get the state machine where this state is running.
 *
 * @return the state machine where this state is running.
 */
PAPSM *PAPSM::State::getStateMachine() const 
{ 
    return stateMachine;
}

//----------------------------------------------------------------------
//--------------------------< PAPSM::WaitPAR >--------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Wait Primary Address Request state.
 *
 * @param stateMachine PAP SM of the state.
 */
PAPSM::WaitPAR::WaitPAR(PAPSM *stateMachine) : State(stateMachine) 
{
}

/**
 * @brief This method is called when a packet is received.
 * If the packet is a PAR, it proposes a Primary Address by sending a PAP packet
 * and changes to WaitPAN state.
 *
 * @param packet received packet
 */
TState* PAPSM::WaitPAR::onPacketReceived(const TControlPacket &packet)
{
    if (packet.getType() == PARPacket::TYPE) {
        
        HypercubeMaskAddress addr = getHypercubeControlLayer()->getPrimaryAddress();

        // If it is proposing a secondary address, don't propose a primary right now to
        // avoid giving the same address.
        if (getHypercubeControlLayer()->isProposingSecondaryAddress()) {
            return NULL;
        }
        
        if (addr.getMask() == addr.getBitLength()) {
            // address space exhausted
            PAPPacket reply(getNode()->getPhysicalAddress(), addr);
            getHypercubeControlLayer()->send(packet.getPhysicalAddress(), reply);
            
        } else {
            // propose an address
            HypercubeAddress propose = addr;
            propose.flipBit(addr.getMask());

            AdditionalAddress proposed(propose, addr.getMask() + 1, 0);     
            
            getStateMachine()->proposedAddresses.clear();
            getStateMachine()->proposedAddresses.push_back(HypercubeMaskAddress(propose, addr.getMask() + 1));
            
            // find if there are reconnect addresses
            vector<AdditionalAddress> reconnect;
            vector<HypercubeMaskAddress> r = getHypercubeControlLayer()->getReconnectAddresses();
            for (int i = 0; i < r.size(); i++) {
                reconnect.push_back(AdditionalAddress(r[i], r[i].getMask()));                
                getStateMachine()->proposedAddresses.push_back(r[i]);
            }
            
            // send the reply
            PAPPacket reply(getNode()->getPhysicalAddress(), addr,  proposed, reconnect);
            getHypercubeControlLayer()->send(packet.getPhysicalAddress(), reply);
        }            
        return getStateMachine()->waitPAN;
    }
    
    return NULL;
}

/**
 * @brief Get the state name
 *
 * @return "WaitPAR"
 */
string PAPSM::WaitPAR::getName() const 
{ 
    return "WaitPAR"; 
}

                
//----------------------------------------------------------------------
//--------------------------< PAPSM::WaitPAN >--------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Wait Primary Address Notification state.
 *
 * @param stateMachine PAP SM of the state.
 */
PAPSM::WaitPAN::WaitPAN(PAPSM *stateMachine) : State(stateMachine) 
{
}

/**
 * @brief Called when the state machine is entering in this state.
 * A timeout is added, in case that the PAN packet never arrives.
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done 
 */ 
TState* PAPSM::WaitPAN::onEnter(TState *from)
{
    addTimeout(HypercubeParameters::WAIT_PAN_TIMEOUT); 
    return NULL;
}

/**
 * @brief Called when the timeout expired.
 * The PAN packet hasn't arrived in the expected time, so move to waitPAR state.
 */
TState* PAPSM::WaitPAN::onTimeout(int id)
{
    return getStateMachine()->waitPAR;
}


/**
 * @brief This method is called when a packet is received.
 * If the packet is a PAN, check if the new connected node has used the address
 * that this node has proposed, and in that case do all the delegation.
 *
 * @param packet received packet
 */
TState* PAPSM::WaitPAN::onPacketReceived(const TControlPacket &packet)
{
    if (packet.getType() == PANPacket::TYPE) {
        HypercubeMaskAddress addr = getHypercubeControlLayer()->getPrimaryAddress();

        int match = -1;
        for (int i=0; i < getStateMachine()->proposedAddresses.size(); i++) {
            if (((HypercubeAddress) getStateMachine()->proposedAddresses[i]) == packet.getPrimaryAddress()) {
                match = i;
                break;
            }
        }
        
        if (match < 0) return getStateMachine()->waitPAR; // debug: estaba en null!
        
        HypercubeMaskAddress acceptedAddress = getStateMachine()->proposedAddresses[match];
        
        // If the other end accepted the proposal for primary address
        if (match == 0) {
            // Confirm that the acceptation has arrived
            PANCPacket reply(getNode()->getPhysicalAddress(), addr);
            getHypercubeControlLayer()->send(packet.getPhysicalAddress(), reply);
            
            // Add it as a neighbour
            Neighbour n(acceptedAddress, packet.getPhysicalAddress());
            n.setType(Neighbour::CHILD);
            getHypercubeControlLayer()->getNeighbours().insert(pair<MACAddress, Neighbour>(packet.getPhysicalAddress(), n));

            // Put a message to indicate that there is a new Route
            getStateMachine()->getNode()->putMessage(new NewRouteMessage(acceptedAddress));           
            
            // Add 1 to the mask
            addr.setMask(addr.getMask() + 1);
            getHypercubeControlLayer()->setPrimaryAddress(addr);

            // Put a message to indicate that address space was given
            AddressGivenMessage *msg = 
                new AddressGivenMessage(acceptedAddress, packet.getPrimaryAddress());            
                    
            getStateMachine()->getNode()->putMessage(msg);           
        } else {
            // The node may have connected with a reconnect address end by us, 
            // in that case it will be cleared.
            if (getHypercubeControlLayer()->eraseReconnectAddress(acceptedAddress)) {
                // Confirm that the acceptation has arrived
                PANCPacket reply(getNode()->getPhysicalAddress(), addr);
                getHypercubeControlLayer()->send(packet.getPhysicalAddress(), reply);

                Neighbour n(packet.getPrimaryAddress(), packet.getPhysicalAddress());
                n.setType(Neighbour::CHILD);
                getHypercubeControlLayer()->getNeighbours().insert(pair<MACAddress, Neighbour>(packet.getPhysicalAddress(), n));

                getStateMachine()->getNode()->putMessage(new NewRouteMessage(acceptedAddress));           
            }
        }
        

        return getStateMachine()->waitPAR;
    }
    
    return NULL;
}

/**
 * @brief Get the state name
 *
 * @return "WaitPAN"
 */
string PAPSM::WaitPAN::getName() const 
{ 
    return "WaitPAN"; 
}

//-----------------------------------------------------------------------
//------------------------------< HBLSM >--------------------------------
//-----------------------------------------------------------------------      

/**
 * @brief Create a Heard Bit Listener State Machine.
 * 
 * @param hcl pointer to the node HypercubeControlLayer
 */ 
HBLSM::HBLSM(HypercubeControlLayer *hcl) : TStateMachine(hcl)
{
    currentState = NULL;    

    listenHB = new ListenHB(this);
    waitSAN = new WaitSAN(this);

    changeState(listenHB);
}


/**
 * @brief Destructor.  Deletes its states.
 */
HBLSM::~HBLSM()
{
    delete listenHB;
    delete waitSAN;
}

/**
 * @brief Get the state machine name
 *
 * @return "HBL"
 */
string HBLSM::getName() const 
{ 
    return "HBL"; 
}

bool HBLSM::isProposingSecondaryAddress() const
{
    return currentState == waitSAN;
}

//----------------------------------------------------------------------
//---------------------------< HBLSM::State >---------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a State for HBL SM.
 *
 * @param stateMachine HBL SM of the state.
 */
HBLSM::State::State(HBLSM *stateMachine) : stateMachine(stateMachine) 
{    
}  

/**
 * @brief Destroy the state.
 * It cancels all the timeouts.
 */              
HBLSM::State::~State() 
{                   
    cancelAllTimeouts();                
}

/**
 * @brief Get the state machine where this state is running.
 *
 * @return the state machine where this state is running.
 */
HBLSM *HBLSM::State::getStateMachine() const 
{ 
    return stateMachine;
}

//-----------------------------------------------------------------------
//--------------------------< HBLSM::ListenHB >--------------------------
//-----------------------------------------------------------------------      
/**
 * @brief Create a Listen Heard Bit state.
 *
 * @param stateMachine HB SM of the state.
 */
HBLSM::ListenHB::ListenHB(HBLSM *stateMachine) : State(stateMachine) 
{
}

/**
 * @brief Called when the state machine is entering in this state.
 * All the neighbours are marked as unactive, so if a node doesn´t recieve a HB,
 * it will end as unactive.
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done  
 */ 
TState* HBLSM::ListenHB::onEnter(TState *from)
{
    map<MACAddress, Neighbour>::iterator it = getHypercubeControlLayer()->getNeighbours().begin();
    map<MACAddress, Neighbour>::iterator end = getHypercubeControlLayer()->getNeighbours().end();
    for (; it != end; it++) {
        it->second.setActive(false);
    }
    
    addTimeout(HypercubeParameters::LISTEN_HB_TIMEOUT); 
    return NULL;    
}


/**
 * @brief Called when the timeout expired.
 * Analyzes the Heard Bits received to mark neighbours as dissapeared, set new ones as
 * adjacent and propose secondayr addresses.
 *
 * @param id id of the expired timeout
 */
TState* HBLSM::ListenHB::onTimeout(int id)
{
    map<MACAddress, Neighbour>::iterator it = getHypercubeControlLayer()->getNeighbours().begin();
    map<MACAddress, Neighbour>::iterator end = getHypercubeControlLayer()->getNeighbours().end();

    HypercubeMaskAddress addr = getHypercubeControlLayer()->getPrimaryAddress();

    for (; it != end; it++) {
        HypercubeAddress neigh = it->second.getPrimaryAddress();

        // Neighbours that didn't send an HB are marked as dissapeared, and a LostRoute message is sent.
        if ((it->second.getType() != Neighbour::NOT_CONNECTED)&& !it->second.isActive()) {
             it->second.setType(Neighbour::DISSAPEARED);
             getStateMachine()->getNode()->putMessage(new LostRouteMessage(it->second.getPrimaryAddress()));                                    
        }
 
        // Found a node that wasn't connected but is adjacent            
        if (it->second.getType() == Neighbour::NOT_CONNECTED &&
            addr.distance(neigh) == 1) {
             it->second.setType(Neighbour::ADJACENT);
             getStateMachine()->getNode()->putMessage(new NewRouteMessage(it->second.getPrimaryAddress()));           

             // Send an HB packet to be sure that the node marked as adjacent knows that.
             HBPacket packet(getStateMachine()->getNode()->getPhysicalAddress(), getHypercubeControlLayer()->getPrimaryAddress());
             getHypercubeControlLayer()->send(MACAddress::BROADCAST, packet);
        }             
        
        // Not Connected nodes to whom no sec address 
        // were proposed are the candidates for sec address, 
        // and the node is not proposing a primary address at this time.
        if (it->second.getType() == Neighbour::NOT_CONNECTED  &&
            !it->second.hasProposedSecondaryAddress() &&
             !getHypercubeControlLayer()->isProposingPrimaryAddress()) {
            
            const vector<HypercubeMaskAddress> &addresses = getHypercubeControlLayer()->getAddresses();
            
            for (int i = 0; i < addresses.size() ;i++) {
                // calculate the distance using just the bits from our mask
                int d = addr.distanceWithMask((HypercubeAddress) neigh);

                // if the distance is 1, we can give it a sec. address
                if (d == 1) {                
                    // calculate the mask that will be needed for the secondary address
                    int maskSA;            
    
                    // find the first 1 after the primary address mask
                    for (maskSA = addr.getMask() + 1; maskSA < neigh.getBitLength(); maskSA++) {
                        if (neigh.getBit(maskSA - 1) == 1) break;
                    }                    
                    
                    // Generate secondary address to propose.
                    HypercubeAddress propose = addr;
                    propose.setBit(maskSA - 1);    
        
                    // send the proposal.
                    it->second.setProposedSecondaryAddress();
                    
                    AdditionalAddress ap(propose, maskSA, 0);     
                    SAPPacket packet(getNode()->getPhysicalAddress(), addr, ap);
                    getHypercubeControlLayer()->send(it->second.getPhysicalAddress(), packet);
                    
                    return getStateMachine()->waitSAN;
                }
            }
        }
    }

    return this;
}

/**
 * @brief This method is called when a packet is received.
 * If the packet is an HB, it marks the neighbour as active, or adds it if it
 * was not in the list before.
 *
 * @param packet received packet
 */
 TState* HBLSM::ListenHB::onPacketReceived(const TControlPacket &packet)
{
    if (packet.getType() == HBPacket::TYPE) {
        map<MACAddress, Neighbour>::iterator end = getHypercubeControlLayer()->getNeighbours().end();
        map<MACAddress, Neighbour>::iterator it = getHypercubeControlLayer()->getNeighbours().find(packet.getPhysicalAddress());
        
        if (it != end) {
            it->second.setActive(true);
            it->second.setLastSeen(Simulator::getInstance()->getTime());
            if (it->second.getPrimaryAddress().getMask() != packet.getPrimaryAddress().getMask()) {
                it->second.setPrimaryAddress(packet.getPrimaryAddress());
                getStateMachine()->getNode()->putMessage(new RouteChangedMaskMessage(packet.getPrimaryAddress()));           
            }

        } else {
            Neighbour n(packet.getPrimaryAddress(), packet.getPhysicalAddress());
            
            getHypercubeControlLayer()->getNeighbours().insert(pair<MACAddress, Neighbour>(packet.getPhysicalAddress(), n));        
        }
    }
    return NULL;
}

/**
 * @brief Get the state name
 *
 * @return "ListenHB"
 */
string HBLSM::ListenHB::getName() const 
{ 
    return "ListenHB"; 
}

//----------------------------------------------------------------------
//--------------------------< HBLSM::WaitSAN >--------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Wait Secondary Address Notification state.
 *
 * @param stateMachine HB SM of the state.
 */
HBLSM::WaitSAN::WaitSAN(HBLSM *stateMachine) : State(stateMachine) 
{
}

/**
 * @brief Called when the state machine is entering in this state.
 * A timeout is added, in case that the SAN packet never arrives.
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done  
 */ 
TState* HBLSM::WaitSAN::onEnter(TState *from)
{
    addTimeout(HypercubeParameters::WAIT_SAN_TIMEOUT);
    return NULL;    
}

/**
 * @brief Called when the timeout expired.
 * The SAN packet hasn't arrived in the expected time, so move to listenHB state.
 */
TState* HBLSM::WaitSAN::onTimeout(int id)
{
    return getStateMachine()->listenHB;
}

/**
 * @brief This method is called when a packet is received.
 * If the packet is a SAN and it is accepted, the neighbour is considered as adjacent.
 * 
 *
 * @param packet received packet
 */
TState* HBLSM::WaitSAN::onPacketReceived(const TControlPacket &packet)
{
    if (packet.getType() == SANPacket::TYPE) {
    
        const SANPacket &san = dynamic_cast<const SANPacket &>(packet);

        // Did the neighbour accept the secondary address?
        if (san.isAccepted()) {
            HypercubeMaskAddress secAddr(san.getProposed().getAddress(), san.getProposed().getMask());
            HypercubeMaskAddress addr = getHypercubeControlLayer()->getPrimaryAddress();

            map<MACAddress, Neighbour>::iterator it = getHypercubeControlLayer()->getNeighbours().find(packet.getPhysicalAddress());

            if ( it!= getHypercubeControlLayer()->getNeighbours().end()) {
                // Mark the neighbour as adjacent and put a message to notify that
                it->second.setType(Neighbour::ADJACENT);
                getStateMachine()->getNode()->putMessage(new NewRouteMessage(it->second.getPrimaryAddress()));                          

                // Send an HB packet to be sure that the node marked as adjacent knows that.
                HBPacket packet(getStateMachine()->getNode()->getPhysicalAddress(), getHypercubeControlLayer()->getPrimaryAddress());
                getHypercubeControlLayer()->send(MACAddress::BROADCAST, packet);

            } else {
                // The neighbour was not found, may be it got disconnected, so go back to Listen HB
                return getStateMachine()->listenHB;                 
            }

            // if we already have the proposed secondary address, don't need to add it.
            const vector<HypercubeMaskAddress> &addresses = getHypercubeControlLayer()->getAddresses();            
            for (int i = 1; i < addresses.size() ;i++) {
                if (addresses[i].contains(secAddr)) {
                    return getStateMachine()->listenHB; 
                }
            }           

            // Add the secondary address to the list.
            getHypercubeControlLayer()->addSecondaryAddress(secAddr);
            
            // Create additional secondary addresses to cover holes in address space
            int bits = secAddr.getMask() - addr.getMask();
            addr.setMask(secAddr.getMask());

            for (int i = 2; i < (1 << bits); i++) {
                HypercubeMaskAddress a = addr;
                for (int j = 0; j < bits; j++) {
                    a.setBit(secAddr.getMask() - j - 1, (i & (1 << j)) != 0);
                }
                getHypercubeControlLayer()->addSecondaryAddress(a);
            }

            // Change the primary address to delegate the space
            getHypercubeControlLayer()->setPrimaryAddress(addr);

            
            // Notify that part of the address space was given.
            AddressGivenMessage *msg = 
                new AddressGivenMessage(HypercubeMaskAddress(san.getPrimaryAddress(), secAddr.getMask()), secAddr);
            getStateMachine()->getNode()->putMessage(msg);           
  
        }        
        return getStateMachine()->listenHB;       
    } 
    
    return NULL;
}

/**
 * @brief Get the state name
 *
 * @return "WaitSAN"
 */
string HBLSM::WaitSAN::getName() const 
{ 
    return "WaitSAN"; 
}

//----------------------------------------------------------------------
//------------------------------< MainSM >------------------------------
//----------------------------------------------------------------------      

/**
 * @brief Create a Main State Machine.
 * It creates their states and registers for listening messages.
 *
 * @param hcl pointer to the node HypercubeControlLayer
 */ 
MainSM::MainSM(HypercubeControlLayer *hcl) : TStateMachine(hcl)
{
    disconnected = new Disconnected(this);
    waitPAP = new WaitPAP(this);
    waitPANC = new WaitPANC(this);
    waitReadyForDisc = new WaitReadyForDisc(this);
    waitWaitMe = new WaitWaitMe(this);
    stableAddress = new StableAddress(this);
    
    getNode()->registerMessageListener(JoinNetworkMessage::TYPE, this);
    getNode()->registerMessageListener(LeaveNetworkMessage::TYPE, this);
    getNode()->registerMessageListener(WaitMeMessage::TYPE, this);
    getNode()->registerMessageListener(ReadyForDiscMessage::TYPE, this);
    
    currentState = disconnected;
}

/**
 * @brief Destroy the machine states.
 */
MainSM::~MainSM()
{
    delete disconnected;
    delete waitPAP;
    delete waitPANC;
    delete waitReadyForDisc;
    delete waitWaitMe;
    delete stableAddress;    
}

/**
 * @brief Get the state machine name
 *
 * @return "Main"
 */
string MainSM::getName() const 
{ 
    return "Main"; 
}

/**
 * @brief Enable HB.
 * If the current state is stable address, it calls onEnter method to start timeout
 */
void MainSM::enableHB() 
{
    if (currentState == stableAddress) {
       stableAddress->onEnter(NULL);
    }
}

//----------------------------------------------------------------------
//---------------------------< MainSM::State >--------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a State for MAIN SM.
 *
 * @param stateMachine MAIN SM of the state.
 */
MainSM::State::State(MainSM *stateMachine) : stateMachine(stateMachine) 
{    
}  

/**
 * @brief Destroy the state.
 * It cancels all the timeouts.
 */              
MainSM::State::~State() 
{                   
    cancelAllTimeouts();                
}

/**
 * @brief Get the state machine where this state is running.
 *
 * @return the state machine where this state is running.
 */
MainSM *MainSM::State::getStateMachine() const 
{ 
    return stateMachine;
}


//----------------------------------------------------------------------
//-----------------------< MainSM::Disconnected >-----------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Disconnected state.
 *
 * @param stateMachine Main SM of the state.
 */        
MainSM::Disconnected::Disconnected(MainSM *stateMachine) : State(stateMachine)
{
}

/**
 * @brief Called when a message is received.
 * If the message is JoinNetwork, it moves to state WaitPAP.
 *
 * @param message the message received. 
 */
TState* MainSM::Disconnected::onMessageReceived(const TMessage *message) 
{
    if (message->getType() == JoinNetworkMessage::TYPE) return getStateMachine()->waitPAP;
    return NULL;
        
}                
/**
 * @brief Get the state name
 *
 * @return "Disconnected"
 */
string MainSM::Disconnected::getName() const 
{ 
    return "Disconnected"; 
}
        
//----------------------------------------------------------------------
//--------------------------< MainSM::WaitPAP >-------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Wait Primary Address Proposal state.
 *
 * @param stateMachine Main SM of the state.
 */
MainSM::WaitPAP::WaitPAP(MainSM *stateMachine) : State(stateMachine)
{
    
}

/**
 * @brief Called when the state machine is entering in this state.
 * It sends a Primary Address Request packets in broadcast, and sets
 * a timeout for waiting for replies.
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done  
 */ 
TState* MainSM::WaitPAP::onEnter(TState *from)
{ 
    // Send the PAR packet.  
    PARPacket packet(getNode()->getPhysicalAddress());
    getHypercubeControlLayer()->send(MACAddress::BROADCAST, packet);

    addTimeout(HypercubeParameters::WAIT_PAP_TIMEOUT); 

    // If it cames from another state (disconnected), it reset the responses and
    // sets the initial number of timeouts before the node considers that there
    // are no other nodes in the network.
    if (from != this) {
        timeoutCount = 5;
        responses.clear();
    }   
    
    return NULL;
}
/**
 * @brief Called when the timeout expires.
 * If it received address proposals, try to connect to them.
 * If it haven't received, it will try for 5 times until it takes the 000.00 address.
 *
 * @param id id of the timeout
 */
TState* MainSM::WaitPAP::onTimeout(int id)
{
    timeoutCount--;
    
    // No replies
    if (responses.size() == 0) {

        if (timeoutCount == 0) {
            // It was the last chance! Take the 000...000 address
            HypercubeMaskAddress addr(dynamic_cast<HypercubeNetwork*>(Simulator::getInstance()->getNetwork())->getAddressLength(), 0);
            getHypercubeControlLayer()->setPrimaryAddress(addr);

            getNode()->putMessage(new ConnectedMessage(addr));
            
            return getStateMachine()->stableAddress;
        } else {
            
            // Still more oportunities to heard from other nodes, so enter again in this state.
            return this;
        }                
    }

    // At least one reply received!!    
    // Check if any of the replies has an address proposal
    bool available = false;
    for (int i = 0; i < responses.size(); i++)
    {
        if (!responses[i].isAddressExhausted()) {
            available = true;
            break;
        }
    }
    
    if (!available) {
        // All the neighbours said that they have their address space exhausted.
        if (timeoutCount == 0) {
            // No more chances, no way to connect.
            getNode()->putMessage(new CantConnectMessage("Address Space Exhausted"));
            Simulator::getInstance()->notify("node.cantConnect", getNode());               
            return getStateMachine()->disconnected;
            
        } else {
            
            // Still more oportunities to heard from nodes that have available addresses
            return this;
        }
        
    }
    

    // can use a reconnect address?
    int bestI = -1;
    int bestJ = -1;    
    int bestAdy = 1;  // there should be at least 2 adyacent (ie the one sending and one more)
    int bestMask = -1;
     
    // find the best reconnection address
    for (int i = 0; i < responses.size(); i++) {
        vector<AdditionalAddress> v = responses[i].getReconnect();
        
        // for each reconnection address
        for (int j = 0; j < v.size(); j++) {
            
            // Count how many adyacent neighbours has the reconnection address
            int adyCount = 0;
            for (int k = 0; k < responses.size(); k++) 
                if(v[j].getAddress().distance(responses[k].getPrimaryAddress()) == 1) adyCount++;
                
            
            if ((adyCount > bestAdy) || ((adyCount == bestAdy) && (v[j].getMask() < bestMask))  ) {
                bestI = i;
                bestJ = j;
                bestAdy = adyCount;
                bestMask = v[j].getMask();
            }                        
        }
    }
        

    // reconnection address found?
    if (bestI >= 0) {
        AdditionalAddress ra = responses[bestI].getReconnect()[bestJ];
        HypercubeMaskAddress myAddr(ra.getAddress(), ra.getMask());
        getHypercubeControlLayer()->setPrimaryAddress(myAddr);
        
        
    } else {           
        // no reconnection address, so choose the best primary address
        int smallestMask = 1000;
        for (int i = 0; i < responses.size(); i++) {
            if (!responses[i].isAddressExhausted() && responses[i].getProposed().getMask() < smallestMask) {
                smallestMask = responses[i].getProposed().getMask();
                bestI = i;
            }
       }
        
        HypercubeMaskAddress myAddr(responses[bestI].getProposed().getAddress(), responses[bestI].getProposed().getMask());
        getHypercubeControlLayer()->setPrimaryAddress(myAddr);
    }
    
    // Add all the neighbours that replied to the neighbour list.
    for (int i = 0; i < responses.size(); i++) {
        Neighbour n(responses[i].getPrimaryAddress(), responses[i].getPhysicalAddress());
        n.setType(i == bestI? Neighbour::PARENT : Neighbour::NOT_CONNECTED);
        
        getHypercubeControlLayer()->getNeighbours().insert(pair<MACAddress, Neighbour>(responses[i].getPhysicalAddress(), n));
    }
        
    return getStateMachine()->waitPANC;

}

/**
 * @brief Called when a packet was received.
 * It just stores the PAP packets received for processing them when the timeout arrives.
 *
 * @param packet the packet received
 */
TState* MainSM::WaitPAP::onPacketReceived(const TControlPacket &packet)
{
    if (packet.getType() == PAPPacket::TYPE) {
        responses.push_back(dynamic_cast<const PAPPacket &>(packet));
    }
    return NULL;
}              
 
/**
 * @brief Get the state name
 *
 * @return "WaitPAP"
 */
string MainSM::WaitPAP::getName() const 
{ 
    return "WaitPAP"; 
}

//----------------------------------------------------------------------
//--------------------------< MainSM::WaitPANC >--------------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Wait Primary Address Notification Confirmation state.
 *
 * @param stateMachine Main SM of the state.
 */
MainSM::WaitPANC::WaitPANC(MainSM *stateMachine) : State(stateMachine)
{
}

/**
 * @brief Called when the state machine is entering in this state.
 * A PAN packet is sent, and a timeout is started, in case that the PANC is never received.
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done.
 */ 
TState* MainSM::WaitPANC::onEnter(TState *from)
{
    PANPacket packet(getNode()->getPhysicalAddress(), getHypercubeControlLayer()->getPrimaryAddress());
    getHypercubeControlLayer()->send(MACAddress::BROADCAST, packet);
    addTimeout(HypercubeParameters::WAIT_PANC_TIMEOUT); 
    
    return NULL;
}

/** 
 * @brief Timeout for receiving PANC packets.
 * It goes back to WaitPAP state.
 *
 * @param id id of the timeout
 */
TState* MainSM::WaitPANC::onTimeout(int id)
{
    return getStateMachine()->waitPAP;
}

/**
 * @brief Called when a packet is received.
 * If the packet is PANC, then the node is finally connected, and it changes
 * the state to StableAddress.
 *
 * @param packet packet received.
 */
TState* MainSM::WaitPANC::onPacketReceived(const TControlPacket &packet)
{
    if (packet.getType() == PANCPacket::TYPE) {
        getNode()->putMessage(new ConnectedMessage(getHypercubeControlLayer()->getPrimaryAddress(), packet.getPrimaryAddress()));
        return getStateMachine()->stableAddress;
    }
    return NULL;
   
}

/**
 * @brief Get the state name
 *
 * @return "WaitPANC"
 */
string MainSM::WaitPANC::getName() const 
{ 
    return "WaitPANC"; 
}
        
//----------------------------------------------------------------------
//---------------------< MainSM::WaitReadyForDisc >---------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Wait Ready For Disconnection state
 *
 * @param stateMachine Main SM of the state.
 */
MainSM::WaitReadyForDisc::WaitReadyForDisc(MainSM *stateMachine) : State (stateMachine)
{
}

/**
 * @brief Called when the state machine is entering in this state.
 * If there are no processes to be waited for, move to disconnected state.
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done. 
 */ 
TState* MainSM::WaitReadyForDisc::onEnter(TState *from)
{
   if (waiting.empty()) {
        return getStateMachine()->disconnected;
    } 
    
    return NULL;
}

/**
 * @brief Called when the state machine is exiting from this state.
 * It broadcast a Disconnection Packet.
 */
void MainSM::WaitReadyForDisc::onExit()
{
    // Send the disc packet
    DiscPacket packet(getNode()->getPhysicalAddress(), 
        getHypercubeControlLayer()->getPrimaryAddress(), getHypercubeControlLayer()->hasChild());

    getHypercubeControlLayer()->send(MACAddress::BROADCAST, packet);
    
    // Put a message in the node that it got disconnected
    getStateMachine()->getNode()->putMessage(new DisconnectedMessage());           
    State::onExit();
    
}       

/**
 * @brief Called when a message is received.
 * When a ReadyForDisc message arrives, the id is removed from the list, and if the
 * list is empty, move to disconnected state.
 *
 * @param message message received.
 * @return a pointer to the next state
 */
TState* MainSM::WaitReadyForDisc::onMessageReceived(const TMessage *message)
{
    if (message->getType() == ReadyForDiscMessage::TYPE) {
        int id = dynamic_cast<const ReadyForDiscMessage *>(message)->getId();

        // find an erase the id 
        set<int>::iterator it = waiting.find(id);
        if (it != waiting.end()) waiting.erase(it);

        if (waiting.empty()) {
            return getStateMachine()->disconnected;
        }
    }
    
    return NULL;
}
               
/**
 * @brief Add an id to be waited for.
 *
 * @param id id to wait for.
 */               
void MainSM::WaitReadyForDisc::addWaiting(int id)
{
    waiting.insert(id);
}

/**
 * @brief Get the state name
 *
 * @return "WaitReadyForDisc"
 */
string MainSM::WaitReadyForDisc::getName() const 
{ 
    return "WaitReadyForDisc"; 
}
        
//----------------------------------------------------------------------
//-------------------------< MainSM::WaitWaitMe >-----------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Wait Wait Me state
 *
 * @param stateMachine Main SM of the state.
 */
MainSM::WaitWaitMe::WaitWaitMe(MainSM *stateMachine)  : State (stateMachine)
{
}

/**
 * @brief Called when the state machine is entering in this state.
 * A Will Disconnect message is sent in the node, and all the processes have
 * some time for replying with a WaitMe message if they need time to disconnect.
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done  
 */ 
TState* MainSM::WaitWaitMe::onEnter(TState *from)
{
    getStateMachine()->getNode()->putMessage(new WillDisconnectMessage());           
    addTimeout(HypercubeParameters::WAIT_WAITME_TIMEOUT);
    return NULL;    
}

/**
 * @brief Timeout expired.  It moves to Wait Ready For Disc state.
 *
 * @param id id of the timeout
 * @return a pointer to the next state or NULL if no state change must be done  
 */
TState* MainSM::WaitWaitMe::onTimeout(int id)
{
    return getStateMachine()->waitReadyForDisc;
}

/**
 * @brief Called when a message si received.
 * If a WaitMe message is received, it is added to the list.
 * If a ReadyForDisc is received, it is erased from the list.
 *
 * @param message the received message.
 * @return a pointer to the next state or NULL if no state change must be done  
 */
TState* MainSM::WaitWaitMe::onMessageReceived(const TMessage *message)
{
    if (message->getType() == WaitMeMessage::TYPE) {
        getStateMachine()->waitReadyForDisc->addWaiting(dynamic_cast<const WaitMeMessage*>(message)->getId());
    }

    // If it receives a Ready For Disc, send it to waitReadyForDisc state so that it can erase it from the list.
    if (message->getType() == ReadyForDiscMessage::TYPE) {
        getStateMachine()->waitReadyForDisc->onMessageReceived(message);
    }
    return NULL;
}


/**
 * @brief Get the state name
 *
 * @return "WaitWaitMe"
 */
string MainSM::WaitWaitMe::getName() const 
{ 
    return "WaitWaitMe"; 
}
        
//----------------------------------------------------------------------
//-----------------------< MainSM::StableAddress >----------------------
//----------------------------------------------------------------------      
/**
 * @brief Create a Stable Address state.
 *
 * @param stateMachine Main SM of the state.
 */

MainSM::StableAddress::StableAddress(MainSM *stateMachine) 
    : State (stateMachine)
{
}

/**
 * @brief Called when it enters to this state.
 * It set ups a timeout for sending Heard Bits
 *
 * @param from the state where the state machine came from.
 * @return a pointer to the next state or NULL if no state change must be done  
 */
TState* MainSM::StableAddress::onEnter(TState *from)
{
    addTimeout(HypercubeParameters::HEARD_BIT_PERIOD);

    return NULL;        
}

/**
 * @brief Heard Bit timeout expired.
 * It broadcast a Heard Bit packet.
 *
 * @param id id of the expired timeout.
 */
TState* MainSM::StableAddress::onTimeout(int id)
{
    if (!getHypercubeControlLayer()->isHBEnabled()) return NULL;

    HBPacket packet(getNode()->getPhysicalAddress(), getHypercubeControlLayer()->getPrimaryAddress());
    getHypercubeControlLayer()->send(MACAddress::BROADCAST, packet);
    return this;    
}

/**
 * @brief Called when a packet is received.
 * It accepts SAP (Secondary Address Proposal) packets, in which case it analyses whether to accept the
 * secondary address, and Disc packets from nodes that got disconnected.
 *
 * @packet packet received.
 * @return a pointer to the next state or NULL if no state change must be done  
 */ 
TState* MainSM::StableAddress::onPacketReceived(const TControlPacket &packet)
{
    if (packet.getType() == SAPPacket::TYPE) {
        const SAPPacket &sap = dynamic_cast<const SAPPacket &>(packet);
        HypercubeMaskAddress proposed(sap.getProposed().getAddress(), sap.getProposed().getMask());
        
        // a secondary address is accepted if it's adjacent.
        bool accept = proposed.distanceWithMask(getHypercubeControlLayer()->getPrimaryAddress()) == 1;
        
        // notify if the secondary address was accepted or not
        SANPacket san(getNode()->getPhysicalAddress(), getHypercubeControlLayer()->getPrimaryAddress(),
                    accept, sap.getProposed());
        
        getHypercubeControlLayer()->send(packet.getPhysicalAddress(), san);

        // if it was accepted
        if (accept) {
            // find the neighbour...
            map<MACAddress, Neighbour>::iterator it = getHypercubeControlLayer()->getNeighbours().find(packet.getPhysicalAddress());

            // ...and mark as adjacent
            if (it!= getHypercubeControlLayer()->getNeighbours().end()) {
                it->second.setType(Neighbour::ADJACENT);
                getStateMachine()->getNode()->putMessage(new NewRouteMessage(it->second.getPrimaryAddress()));                       

                // Send an HB packet to be sure that the node marked as adjacent knows that.
                HBPacket packet(getStateMachine()->getNode()->getPhysicalAddress(), getHypercubeControlLayer()->getPrimaryAddress());
                getHypercubeControlLayer()->send(MACAddress::BROADCAST, packet);                
            }            
        }       
        return NULL;
    }

    if (packet.getType() == DiscPacket::TYPE) {
        const DiscPacket &disc = dynamic_cast<const DiscPacket &>(packet);
        
        map<MACAddress, Neighbour>::iterator end = getHypercubeControlLayer()->getNeighbours().end();
        map<MACAddress, Neighbour>::iterator it = getHypercubeControlLayer()->getNeighbours().find(packet.getPhysicalAddress());
        
        if (it != end) {
            getStateMachine()->getNode()->putMessage(new LostRouteMessage(it->second.getPrimaryAddress()));                       

            // recover the address if possible
            if ((it->second.getType() == Neighbour::CHILD) && !disc.hasChild() ) {
                recoverAddress(it->second.getPrimaryAddress());
            }
            it->second.setType(Neighbour::DISCONNECTED);

            getHypercubeControlLayer()->addReconnectAddress(disc.getPrimaryAddress());
        }         
    }
    return NULL;
}
               
/**
 * @brief Helper method to recover addresses when a node is disconnected, by checking
 * if the space can be defragmented.
 *
 * @param child the child that got disconnected.
 */               
void MainSM::StableAddress::recoverAddress(const HypercubeAddress &child) {
    HypercubeMaskAddress addr = getHypercubeControlLayer()->getPrimaryAddress();    
    HypercubeAddress recoveredMask = getHypercubeControlLayer()->getRecoveredMask();

    int i ;
    for (i = 0;i < addr.getBitLength(); i++) {
        if (addr.getBit(i) != child.getBit(i)) break;
    }
    
    recoveredMask.setBit(i);
    
    int j = addr.getMask() - 1;
    while (j >= 0 && recoveredMask.getBit(j) == 1) {
        recoveredMask.setBit(j, 0);
        j--;
    }
    
    addr.setMask(j+1);
    getHypercubeControlLayer()->setPrimaryAddress(addr);
    getHypercubeControlLayer()->setRecoveredMask(recoveredMask);
}
               
/**
 * @brief Called when a message is received.
 * If the message is LeaveNetwork, it changes to WaitWaitMe state.
 *
 * @param message message received
 * @return a pointer to the next state or NULL if no state change must be done  
 */
TState* MainSM::StableAddress::onMessageReceived(const TMessage *message)
{
    if (message->getType() == LeaveNetworkMessage::TYPE) {
        return getStateMachine()->waitWaitMe;
    }
    
    return NULL;    
}
               
/**
 * @brief Get the state name
 *
 * @return "StableAddress"
 */
string MainSM::StableAddress::getName() const 
{ 
    return "StableAddress"; 
}
      
}
}

