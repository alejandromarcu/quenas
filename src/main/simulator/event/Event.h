#ifndef _EVENT_H_
#define _EVENT_H_

#include "DataUnit.h"
#include "TPhysicalLayer.h"
#include "TConnection.h"
#include "BitStream.h"
#include "common.h"
#include "Units.h"
#include "Event.h"
#include "Message.h"
#include "Command.h"

namespace simulator {
    namespace event {

using namespace simulator::dataUnit;
using namespace simulator::layer;
using namespace simulator::message;
using namespace simulator::command;

/*****************************************************************************/
/**
 * @brief Base class for any event.
 *
 * Each event will be executed by the framework at the specified time, and
 * optionally rescheduled periodically.
 */
class TEvent {
    public:
        TEvent(Time time, Time period = 0);
        virtual ~TEvent();
        void addTime(Time time);
        void runEvent();
        Time getTime() const;
        Time getPeriod() const;
        long getSequence() const;        


        /**
         * @brief Method executed when this event is scheduled.
         *
         * @param time current time.
         */
        virtual void run(Time time) = 0;

    private:
        /// Time where the event will run
        Time time;
        
        /// Repetition period, or non-positive if it's not periodic.
        Time period;
        
        /// Sequence number to break ties of equal times.
        long sequence;

        /// This value is incremented each time an instance is 
        // created to assign the sequence number.
        static long sequenceGenerator;
};

/*****************************************************************************/
/**
 * @brief Event used to send a Bit Stream from a Physical Layer to a connection.
 * When the event is raised, the connection receives the bit stream.
 */
class SendBitStreamEvent : public TEvent {
    public:
        SendBitStreamEvent(Time time, TPhysicalLayer *from, TConnection *connection, const BitStream &bitStream);
        virtual void run(Time time);
        
    private:
        /// source physical layer
        TPhysicalLayer *from;
        
        /// destination connection
        TConnection *connection;
        
        /// bit stream to send
        BitStream bitStream;
};


/*****************************************************************************/
/**
 * @brief Event used to receive a Bit Stream in a Physical Layer from a connection.
 * When the event is raised, the physical layer receives the bit stream.
 */
class ReceiveBitStreamEvent : public TEvent {
    public:
        ReceiveBitStreamEvent(Time time, TPhysicalLayer *destination, const BitStream &bitStream);
        virtual void run(Time time);
        
    private:
        /// destination physical layer
        TPhysicalLayer *destination;
        
        /// bit stream to receive
        BitStream bitStream;
};

/*****************************************************************************/

/**
 * @brief Interface for classes willing to receive timeouts.
 */
class TTimeoutTarget {
    public:
        /**
         * @brief This method will be called when the timeout is over.
         *
         * @param id the id of the time out.
         */
        virtual void onTimeout(int id) = 0;    
};

/*****************************************************************************/

/**
 * @brief Event that raises a time out.
 */
class TimeoutEvent : public TEvent {
    public:
        TimeoutEvent(Time time, TTimeoutTarget *target, int id);
        virtual void run(Time time);
        void cancel();
        
    private:
        /// The object where the timeout will be raised.
        TTimeoutTarget *target;
        
        /// Id of the timeout to raise.
        int id;
        
        /// Whether this time out was cancelled.
        bool isCancelled;
        
};


/*****************************************************************************/

/**
 * @brief This event is for receiving a message internal to the node.
 */
class ReceiveMessageEvent : public TEvent {
    public:
        ReceiveMessageEvent(Time time, TMessageReceiver *destination, TMessage *message);
        virtual void run(Time time);
        
    private:
        /// Object receiving the message
        TMessageReceiver *destination;
        
        /// Message to receive
        TMessage *message;
};


/*****************************************************************************/

/**
 * @brief Event used to run a command.
 */
class CommandRunnerEvent : public TEvent {
    public:
        CommandRunnerEvent(Time time, TCommandRunner *destination, const string &command, Time period = 0);
        virtual void run(Time time);
        
    private:
        /// Where the commanad is run
        TCommandRunner *destination;
        
        /// Command to run
        string command;
};

}
}

/*****************************************************************************/
    
namespace std {

using namespace simulator::event;

// Specialization for the less structure for working with "Event *" so that
// it compares the time, and in case of similar times, it compares the sequence number.
template<>
struct less<TEvent *> : public binary_function<TEvent *, TEvent *, bool> {
    bool operator()(const TEvent *x, const TEvent *y) const {
        if (x->getTime() > y->getTime()) return true;
        if (x->getTime() < y->getTime()) return false;
        return x->getSequence() > y->getSequence();
    }
};
}

#endif
