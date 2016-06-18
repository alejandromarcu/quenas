#ifndef _MESSAGE_H_
#define _MESSAGE_H_


#include <string>
#include "HypercubeAddress.h"
#include "HypercubeMaskAddress.h"
#include "UniversalAddress.h"
#include "Notification.h"

namespace simulator {
    namespace message {

using namespace simulator::address;
using namespace simulator::notification;
using namespace std;

/**
 * @brief Interface for all the message implementations.
 * It stores a use counter, so that many events can share
 * the same message and when the use counter reaches 0,
 * it can be deleted.
 */
class TMessage : public TQueryable {
    public:
        /**
         * @brief Create a new message.
         */
        TMessage() : useCount(0) {};
        
        /**
         * @brief Virtual destructor. Does nothing.
         */
        virtual ~TMessage() {};
        
        /** 
         * @brief Get the type of message.
         *
         * @return the type of message.
         */
        virtual string getType() const = 0;

        /**
         * @brief Clone this message.
         *
         * @return a clone of the message.
         */
        virtual TMessage* clone() const = 0;
        
        /**
         * @brief Increment by one use count for this message.
         */
        void incUseCount() { useCount++; }

        /**
         * @brief Decrement by one use count for this message.
         */
        void decUseCount() { useCount--; }
        
        /**
         * @brief Get the use count for this message.
         *
         * @return the use count for this message.
         */ 
        int getUseCount() const { return useCount; }

        virtual QueryResult *query(const vector<string> *options = NULL) const { return NULL;    }        
    private:
        /// Use count for this message.
        int useCount;
};

/**
 * @brief Interface for classes willing to receive messages.
 */
class TMessageReceiver {
    public:
        /**
         * @brief Method called to indicate that a message was received.
         *
         * @param message the message received.
         */
        virtual void onMessageReceived(const TMessage *message) = 0;
};

/**
 * @brief Message used to indicate that the node has just been connected.
 */
class ConnectedMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        ConnectedMessage(const HypercubeMaskAddress &primaryAddress);
        ConnectedMessage(const HypercubeMaskAddress &primaryAddress, const HypercubeMaskAddress &parent);
        HypercubeMaskAddress getPrimaryAddress() const;
        HypercubeMaskAddress getParentAddress() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;

    private:
        /// Primary address acquired
        HypercubeMaskAddress primaryAddress;
        
        /// Parent address of this node
        HypercubeMaskAddress parent;        
};

/**
 * @brief Message used to indicate that the node has just given
 * a part of his address space
 */
class AddressGivenMessage : public TMessage {
    public:
        /// String identification for this type of message.
        const static string TYPE;

        AddressGivenMessage(const HypercubeMaskAddress &givenAddress, const HypercubeAddress &destination);
        HypercubeMaskAddress getGivenAddress() const;
        HypercubeAddress getDestination() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;
        
    private:
        /// Space that was given
        HypercubeMaskAddress givenAddress;
        
        /// Who received it
        HypercubeAddress destination;        
};


/**
 * @brief Message used to indicate that the node has found a new route
 * (i.e. there is a new neighbour).
 */
class NewRouteMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        NewRouteMessage(const HypercubeMaskAddress &route);
        HypercubeMaskAddress getRoute() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;
        
    private:
        /// new route
        HypercubeMaskAddress route;
};

/**
 * @brief Message used to indicate that a route was lost
 * (i.e. a neighbour got disconnected).
 */
class LostRouteMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        LostRouteMessage(const HypercubeMaskAddress &route);
        HypercubeMaskAddress getRoute() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;

        
    private:
        /// lost route
        HypercubeMaskAddress route;
};

/**
 * @brief Message used to indicate that a route has changed its mask
 */
class RouteChangedMaskMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        RouteChangedMaskMessage(const HypercubeMaskAddress &route);
        HypercubeMaskAddress getRoute() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;

        
    private:
        /// new address of the route (just the mask should be different than before)
        HypercubeMaskAddress route;
};

/**
 * @brief Message used to indicate that the node couldn't connect to network.
 */
class CantConnectMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        CantConnectMessage(const string &reason);
        string getReason() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;
        
    private:
        /// the reason why the connection was not possible
        string reason;
};

/**
 * @brief Message used to ask for waiting to disconnect.  When the process is
 * ready to disconnect, it should send a ReadyForDiscMessage with the same id.
 */
class WaitMeMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        WaitMeMessage(long id);
        long getId() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;
        
    private:
        /// identification of wait me
        long id;
};

/**
 * @brief Message used to confirm that the process is ready for disconnection,
 * after having asked for waiting to disconnect with WaitMeMessage.
 */
class ReadyForDiscMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        ReadyForDiscMessage(long id);
        long getId() const;
        virtual string getType() const;
        virtual TMessage* clone() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;
        
    private:
        /// identification of previous wait me
        long id;
};

/**
 * @brief Message indicating that the node is willing to join the network.
 */
class JoinNetworkMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        /// Get the type of message (JOIN_NETWORK)
        virtual string getType() const { return TYPE; }

        /// Clone this message.
        virtual TMessage* clone() const { return new JoinNetworkMessage(); }
};

/**
 * @brief Message indicating that the node is willing to leave the network.
 */
class LeaveNetworkMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        /// Get the type of message (LEAVE_NETWORK)
        virtual string getType() const { return TYPE; }

        /// Clone this message.
        virtual TMessage* clone() const { return new LeaveNetworkMessage(); }

};

/**
 * @brief Message indicating that the node was just disconnected.
 */
class DisconnectedMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        /// Get the type of message (DISCONNECTED)
        virtual string getType() const { return TYPE; }

        /// Clone this message.
        virtual TMessage* clone() const { return new DisconnectedMessage(); }

};

/**
 * @brief Message indicating that the node will disconnect.  Any process willing
 * to do something beofre disconnection  needs to reply wit a WaitMeMessage.
 */
class WillDisconnectMessage : public TMessage {
    public:
        /// String identification for this type of message.        
        const static string TYPE;

        /// Get the type of message (WILL_DISCONNECT)
        virtual string getType() const { return TYPE; }

        /// Clone this message.
        virtual TMessage* clone() const { return new WillDisconnectMessage(); }
};

// end namespaces
}
}

#endif
