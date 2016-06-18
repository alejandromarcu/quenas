#include <iostream>

#include "Event.h"
#include "common.h"
#include "Simulator.h"

namespace simulator {
    namespace event {

using namespace std;
using namespace simulator;

long TEvent::sequenceGenerator = 0;

//----------------------------------------------------------------------
//------------------------------< TEvent >------------------------------
//----------------------------------------------------------------------
/**
 * @brief Create an event.
 *
 * @param time time when the event runs.
 * @param period repetition period or 0 to make it non periodic.
 */
TEvent::TEvent(Time time, Time period) : time(time), period(period)
{
    sequence = sequenceGenerator++;
}

/**
 * @brief Destroy the event.
 */
TEvent::~TEvent()
{
}

/**
 * @brief Increment the scheduled time.
 *
 * @param time amount of time to add.
 */
void TEvent::addTime(Time time)
{
    this->time += time;
}

/**
 * @brief Run the event by delegating to abstract method run, and 
 * reschedule if periodic.
 */
void TEvent::runEvent()
{
    run(time);
     
    if (period.getValue() > 0) {
        this->time += period;
    }
}

/**
 * @brief Get the time scheduled for running.
 *
 * @return the time scheduled for running.
 */
Time TEvent::getTime() const
{
    return time;
}

/**
 * @brief Get the repetition period.
 *
 * @return the repetition period.
 */
Time TEvent::getPeriod() const
{
    return period;
}


/**
 * @brief Get the sequence number used to break time ties.
 *
 * @return the sequence number used to break time ties.
 */
long TEvent::getSequence() const 
{
    return sequence;
}

//----------------------------------------------------------------------
//------------------------< SendBitStreamEvent >------------------------
//----------------------------------------------------------------------

/**
 * @brief Create a SendBitStreamEvent.
 *
 * @param time time to run.
 * @param from Physical layer sending the bit stream.
 * @param connection the connection that will receive the bit stream.
 * @param bitStream bit stream to send.
 */
SendBitStreamEvent::SendBitStreamEvent(Time time, TPhysicalLayer *from, TConnection *connection, const BitStream &bitStream) 
    : TEvent(time), from(from), connection(connection), bitStream(bitStream)
{
}
   
/**
 * @brief This method is called by the framework to run the event.
 * The connection receives the bit stream.
 *
 * @param time the current time.
 */         
void SendBitStreamEvent::run(Time time)
{
    connection->transport(from, bitStream);
}

//----------------------------------------------------------------------
//-----------------------< ReceiveBitStreamEvent >----------------------
//----------------------------------------------------------------------

/**
 * @brief Create a ReceiveBitStreamEvent.
 *
 * @param time time to run.
 * @param destination Physical layer receiving the bit stream.
 * @param bitStream bit stream to receive.
 */
ReceiveBitStreamEvent::ReceiveBitStreamEvent(Time time, TPhysicalLayer *destination, const BitStream &bitStream) 
    : TEvent(time), destination(destination), bitStream(bitStream)
{
}
        
/**
 * @brief This method is called by the framework to run the event.
 * The Physical Layer receives the bit stream.
 *
 * @param time the current time.
 */         
void ReceiveBitStreamEvent::run(Time time)
{
    destination->receive(bitStream);
}

//----------------------------------------------------------------------
//---------------------------< TimeoutEvent >---------------------------
//----------------------------------------------------------------------

/**
 * @brief Create a Timeout event.
 *
 * @param time time when the timeout will be raised.
 * @param target object where the timeout will be raised.
 * @param id id of the timeout.
 */
TimeoutEvent::TimeoutEvent(Time time, TTimeoutTarget *target, int id) 
    : TEvent(time), target(target), id(id)
{
    isCancelled=false;
}

/**
 * @brief This method is called by the framework to run the event.
 *
 * @param time the time when this event is running.
 */
void TimeoutEvent::run(Time time)
{
    if (!isCancelled) target->onTimeout(id);
}

/**
 * @brief Cancel this timeout.
 */
void TimeoutEvent::cancel()
{
    isCancelled = true;
}

//----------------------------------------------------------------------
//----------------------< ReceiveMessageEvent >-------------------------
//----------------------------------------------------------------------

/**
 * @brief Create a Receive Message event.
 *
 * @param time the time where the message will be received.
 * @param destination the object that will receive the message.
 * @param message the message to send.  This object will take care of deleting the message
 * once all the destinations got the message.
 */
ReceiveMessageEvent::ReceiveMessageEvent(Time time, TMessageReceiver *destination, TMessage *message)
    : TEvent(time), destination(destination), message(message)
{
    message->incUseCount();
}

/**
 * @brief This method is called by the framework when the message must be received.
 * It calls onMessageReceived on the destination, and if the useCount reaches 0,
 * the message is deleted.
 * @param time the time when this event is running.
 */
void ReceiveMessageEvent::run(Time time)
{
    destination->onMessageReceived(message);
    message->decUseCount();

    if (message->getUseCount() == 0) delete message;
}


//----------------------------------------------------------------------
//------------------------< CommandRunnerEvent >------------------------
//----------------------------------------------------------------------

/**
 * @brief Create a Command Runner event.
 *
 * @param time the time when the command will be run.
 * @param destination the object where the command will run.
 * @param command command to run.
 * @param period the repetition period for the event.
 */
CommandRunnerEvent::CommandRunnerEvent(Time time, TCommandRunner *destination, const string &command, Time period)
    : TEvent(time, period), destination(destination), command(command)
{
}

/**
 * @brief This method is called by the framework when the command must be run.
 * 
 * @param time the time when this event is running.
 */
void CommandRunnerEvent::run(Time time)
{
    Simulator::getInstance()->exec(destination, command);
}



}
}


