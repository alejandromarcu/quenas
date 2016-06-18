#ifndef _STATE_MACHINES_H_
#define _STATE_MACHINES_H_

#include <vector>
#include <iterator>
#include <set>

#include "common.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "HypercubeNetwork.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "Simulator.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;
using namespace simulator;
      
/**
 * @brief Base class for all the states of Hypercube state machines.
 */
class TState {
      public:
          virtual TState* onEnter(TState *from);
          virtual void onExit();
          virtual TState* onTimeout(int id);
          virtual TState* onMessageReceived(const TMessage *message);
          virtual TState* onPacketReceived(const TControlPacket &packet);
          virtual string getName() const = 0;
          
      protected:
          int addTimeout(Time elapsed);
          void cancelAllTimeouts();

          virtual TStateMachine *getStateMachine() const = 0;
          HypercubeControlLayer *getHypercubeControlLayer() const;
          virtual HypercubeNode *getNode() const;
          
      private:
          /// The id's of the timeouts that are currently in use.  
          vector<int> timeouts;
          
};
      
/**
 * @brief Base class for Hypercube state machines.
 */      
class TStateMachine : public TMessageReceiver, public TTimeoutTarget {
    public:
        TStateMachine(HypercubeControlLayer *hcl);
        virtual ~TStateMachine();
        
        virtual void onTimeout(int id);
        virtual void onMessageReceived(const TMessage *message);
        virtual void onPacketReceived(const TControlPacket &packet);
        
        int addTimeout(Time elapsed);
        void cancelTimeout(int id);

        virtual string getName() const = 0;
        HypercubeNode *getNode() const;
        HypercubeControlLayer *getHypercubeControlLayer() const;

    protected:
        void changeState(TState *newState);
   
        /// Pointer to the current state of the SM     
        TState *currentState;
        
    private:
        /// Map from the timeout ids to their event, used to cancel them.
        map<int, TimeoutEvent*> timeouts;
        
        /// HypercubeControlLayer owning this state machine.
        HypercubeControlLayer *hcl;
        
        /// Current timeoutId, increased each time addTimeout is called.
        int timeoutId;
};

/**
 * @brief Primary Address Provider State Machine.
 * This state machine listens for requests of primary addresses
 * and reply them
 */      
class PAPSM : public TStateMachine {
    public:
        PAPSM(HypercubeControlLayer *hcl);
        virtual ~PAPSM();
        virtual string getName() const;
        bool isProposingPrimaryAddress() const;

        /**
         * @brief Base clase for PAP SM states.
         */     
        class State : public TState {
            public:
                State(PAPSM *stateMachine);
                virtual ~State();

            protected:
                virtual PAPSM *getStateMachine() const;

            private:
                /// State Machine where the state is running
                PAPSM *stateMachine;
        };
        
        /**
         * @brief Wait Primary Address Request state.
         *
         * It just waits for a PAR packet to arrive, and moves to WaitPAN state
         * when it arrives.
         */
        class WaitPAR : public State {
            private:
                
            public:
                WaitPAR(PAPSM *stateMachine);
                virtual TState* onPacketReceived(const TControlPacket &packet);
                virtual string getName() const;
        };
        
        /**
         * @brief Wait Primary Address Notification state.
         *
         * It waits for a PAN packet, informing that the nde has chosen a
         * primary address, and then goes back to WaitPAR state.
         */
        class WaitPAN : public State {
            public:
                WaitPAN(PAPSM *stateMachine);
                virtual TState* onEnter(TState *from);
                virtual TState* onTimeout(int id);
                virtual TState* onPacketReceived(const TControlPacket &packet);
                virtual string getName() const;
        };
        
    private:        
        /// WaitPAR state
        WaitPAR *waitPAR;

        /// WaitPAN state
        WaitPAN *waitPAN;
        
        vector<HypercubeMaskAddress> proposedAddresses; //newVersion
};

/** 
 * @brief Heard Bit Listener State Machine.
 * It listens for Heard Bit packets to keep neighbours alive, and proposes
 * secondary addresses if possible.
 */
class HBLSM : public TStateMachine {
    public:
        HBLSM(HypercubeControlLayer *hcl);
        virtual ~HBLSM();
        virtual string getName() const;
        bool isProposingSecondaryAddress() const;
         
        /**
         * @brief Base clase for PAP SM states.
         */     
        class State : public TState {
            public:
                State(HBLSM *stateMachine);
                virtual ~State();
                
            protected:
                virtual HBLSM *getStateMachine() const;

            private:
                /// State Machine where the state is running                
                HBLSM *stateMachine;
        };
        
        /**
         * @brief State where Heard Bit are listened.
         * The nodes that doesn't reply in an amount of time are considered as dead.
         */
        class ListenHB : public State {
            public:
                ListenHB(HBLSM *stateMachine);
                virtual TState* onEnter(TState *from);
                virtual TState* onTimeout(int id);
                virtual TState* onPacketReceived(const TControlPacket &packet);                
                virtual string getName() const;
        };
        
        /**
         * @brief Wait for Secondary Address Notifications.
         */
        class WaitSAN : public State {
            public:
                WaitSAN(HBLSM *stateMachine);
                virtual TState* onEnter(TState *from);
                virtual TState* onTimeout(int id);
                virtual TState* onPacketReceived(const TControlPacket &packet);
                virtual string getName() const;
        };
        
    private:
        /// ListenHB state
        ListenHB *listenHB;
        
        /// WaitSAN state
        WaitSAN *waitSAN;
};


/**
 * @brief Main State Machine.
 * It takes care of connecting and disconnecting from the network.
 */
class MainSM : public TStateMachine {
    public:
        MainSM(HypercubeControlLayer *hcl);
        virtual ~MainSM();
        virtual string getName() const;
        void enableHB();
        
        /**
         * @brief Base clase for Main SM states.
         */     
        class State : public TState {
            public:
                State(MainSM *stateMachine);
                virtual ~State();                

            protected:
                virtual MainSM *getStateMachine() const;

            private:
                /// State Machine where the state is running                
                MainSM *stateMachine;
        };
        
        /**
         * @brief Disconnected state.
         * It waits for a joinNetwork message and moves to WaitPap.
         */
        class Disconnected : public State {
            public:
                Disconnected(MainSM *stateMachine);
                virtual TState* onMessageReceived(const TMessage *message);                
                virtual string getName() const;
        };
        
        /**
         * @brief Wait for Primary Address Proposal State.
         * When entering this state, a Primary Address Request is sent,
         * and it waits for replies.
         */
        class WaitPAP : public State {
            public:
                WaitPAP(MainSM *stateMachine);
                virtual TState* onEnter(TState *from);
                virtual TState* onTimeout(int id);
                virtual TState* onPacketReceived(const TControlPacket &packet);                
                virtual string getName() const;
                
            private:
                /// how many times the timeout has expired
                int timeoutCount;
                
                /// PAP packets received as responses
                vector<PAPPacket> responses;
        };
        
        /**
         * @brief Wait Primary Address Notification Confirmation.
         * When entering this state, it sends a PAN packet and
         * waits for its confirmation.
         */
        class WaitPANC : public State {
            public:
                WaitPANC(MainSM *stateMachine);
                virtual TState* onEnter(TState *from);
                virtual TState* onTimeout(int id);
                virtual TState* onPacketReceived(const TControlPacket &packet);                
                virtual string getName() const;
        };
        
        /**
         * @brief Wait for all the process in the same node that sent an "WaitMe"
         * message to send a ReadyForDisc mesage, indicating that we can proceed.
         */
        class WaitReadyForDisc : public State {
            public:
                WaitReadyForDisc(MainSM *stateMachine);
                virtual TState* onEnter(TState *from);                
                virtual void onExit();
                virtual TState* onMessageReceived(const TMessage *message);                
                void addWaiting(int id);
                virtual string getName() const;
                
            private:
                /// Id of the processes that we are still waiting
                set<int> waiting;
        };
        
        /**
         * @brief All the processes are notified that the node will be disconencted,
         * and in this we wait for WaitMe messages from processes.
         */
        class WaitWaitMe : public State {
            public:
                WaitWaitMe(MainSM *stateMachine);
                virtual TState* onEnter(TState *from);
                virtual TState* onTimeout(int id);
                virtual TState* onMessageReceived(const TMessage *message);                
                virtual string getName() const;
        };
        
        /**
         * @brief The node has acquired an address, and will remain in this state
         * until it is asked for disconnection.
         */
        class StableAddress : public State {
            public:
                StableAddress(MainSM *stateMachine);
                virtual TState* onEnter(TState *from);
                virtual TState* onTimeout(int id);
                virtual TState* onPacketReceived(const TControlPacket &packet);                
                virtual TState* onMessageReceived(const TMessage *message);                
                virtual string getName() const;
                
            private:
                void recoverAddress(const HypercubeAddress &child);                
        };
        
    private:
        /// Disconnected state
        Disconnected *disconnected;
        
        /// WaitPAP state
        WaitPAP *waitPAP;
        
        /// WaitPANC state
        WaitPANC *waitPANC;
        
        /// WaitReadyForDisc state
        WaitReadyForDisc *waitReadyForDisc;
        
        /// WaitWaitMe state
        WaitWaitMe *waitWaitMe;
        
        /// StableAddress state
        StableAddress *stableAddress;
};
          
}
}

#endif
