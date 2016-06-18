#include "Message.h"

namespace simulator {
        namespace message {

using namespace simulator::address;

const string ConnectedMessage::TYPE = "CONNECTED";
const string AddressGivenMessage::TYPE = "ADDRESS_GIVEN";
const string NewRouteMessage::TYPE = "NEW_ROUTE";
const string LostRouteMessage::TYPE = "LOST_ROUTE";
const string RouteChangedMaskMessage::TYPE = "ROUTE_CHANGED_MASK";
const string CantConnectMessage::TYPE = "CANT_CONNECT";
const string WaitMeMessage::TYPE = "WAIT_ME";
const string ReadyForDiscMessage::TYPE = "READY_FOR_DISC";
const string JoinNetworkMessage::TYPE = "JOIN_NETWORK";
const string LeaveNetworkMessage::TYPE = "LEAVE_NETWORK";
const string DisconnectedMessage::TYPE = "DISCONNECTED";
const string WillDisconnectMessage::TYPE = "WILL_DISCONNECT";

//----------------------------------------------------------------------------
//----------------------------< ConnectedMessage >----------------------------
//----------------------------------------------------------------------------

/**
 * @brief Create a ConnectedMessage.
 *
 * @param primaryAddress the new assigned address.
 */
ConnectedMessage::ConnectedMessage(const HypercubeMaskAddress &primaryAddress) : primaryAddress(primaryAddress), parent("",0)
{
}

/**
 * @brief Create a ConnectedMessage.
 *
 * @param primaryAddress the new assigned address.
 */
ConnectedMessage::ConnectedMessage(const HypercubeMaskAddress &primaryAddress, const HypercubeMaskAddress &parent) 
        : primaryAddress(primaryAddress), parent(parent)
{
}

/**
 * @brief Get the primary address assigned.
 *
 * @return the primary address assigned.
 */
HypercubeMaskAddress ConnectedMessage::getPrimaryAddress() const
{
    return primaryAddress;
}

/**
 * @brief Get the address of the parent.
 *
 * @return the address of the parent.
 */
HypercubeMaskAddress ConnectedMessage::getParentAddress() const
{
    return parent;
}

/** 
 * @brief Get the type of message.
 *
 * @return the type of message (CONNECTED)
 */
string ConnectedMessage::getType()  const
{ 
    return TYPE; 
};

/**
 * @brief Clone this message.
 *
 * @return A ConnectedMessage equal to this instance.
 */
TMessage* ConnectedMessage::clone() const
{
    return new ConnectedMessage(primaryAddress);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *ConnectedMessage::query(const vector<string> *options) const 
{
    QueryResult *qr = new QueryResult("message");
    qr->insert("primaryAddress", primaryAddress.toString());
    return qr;            
}        

//---------------------------------------------------------------------------
//--------------------------< AddressGivenMessage >--------------------------
//---------------------------------------------------------------------------

/**
 * @brief Create an AddressGivenMessage message.
 *
 * @param givenAddress the address that the node gave.
 * @param mask the mask for the given address space.
 * @param destination the primary address of the node receiving the address space.
 */
AddressGivenMessage::AddressGivenMessage(const HypercubeMaskAddress &givenAddress, 
            const HypercubeAddress &destination) 
        : givenAddress(givenAddress), destination(destination)
{
}

/**
 * @brief Get the given address space.
 * 
 * @return the given address space.
 */
HypercubeMaskAddress AddressGivenMessage::getGivenAddress() const
{
    return givenAddress;
}

/**
 * @brief Get the primary address of the node receiving the address space.
 * 
 * @return the primary address of the node receiving the address space.
 */
HypercubeAddress AddressGivenMessage::getDestination() const
{
    return destination;
}



/** 
 * @brief Get the type of message.
 *
 * @return the type of message (ADDRESS_GIVEN)
 */
string AddressGivenMessage::getType() const
{ 
    return TYPE; 
};


/**
 * @brief Clone this message.
 *
 * @return an AddressGivenMessage equal to this instance.
 */
TMessage* AddressGivenMessage::clone() const
{
    return new AddressGivenMessage(givenAddress, destination);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *AddressGivenMessage::query(const vector<string> *options) const {
    QueryResult *qr = new QueryResult("message");
    qr->insert("givenAddress", givenAddress.toString());
    qr->insert("destination", destination.toString());
    return qr;            
}        

//----------------------------------------------------------------------------
//----------------------------< NewRouteMessage >------------------------
//----------------------------------------------------------------------------

/**
 * @brief Create a NewRouteMessage.
 *
 * @param route the address for the new route.
 */
NewRouteMessage::NewRouteMessage(const HypercubeMaskAddress &route) : route(route)
{
}

/**
 * @brief Get the address for the new route.
 *
 * @param the address for the new route.
 */
HypercubeMaskAddress NewRouteMessage::getRoute() const
{
    return route;
}

/** 
 * @brief Get the type of message.
 *
 * @return the type of message (NEW_ROUTE)
 */
string NewRouteMessage::getType() const
{
    return TYPE;
}

/**
 * @brief Clone this message.
 *
 * @return a NewRouteMessage equal to this instance.
 */
TMessage* NewRouteMessage::clone() const
{
    return new NewRouteMessage(route);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *NewRouteMessage::query(const vector<string> *options) const {
    QueryResult *qr = new QueryResult("message");
    qr->insert("route", route.toString());
    return qr;            
}        

//----------------------------------------------------------------------------
//------------------------------< LostRouteMessage >--------------------------
//----------------------------------------------------------------------------

/**
 * @brief Create a LostRouteMessage.
 *
 * @param route the address for the lost route.
 */
LostRouteMessage::LostRouteMessage(const HypercubeMaskAddress &route) : route(route)
{
}

/**
 * @brief Get the address for the lost route.
 *
 * @param the address for the lost route.
 */
HypercubeMaskAddress LostRouteMessage::getRoute() const
{
    return route;
}

/** 
 * @brief Get the type of message.
 *
 * @return the type of message (LOST_ROUTE)
 */
string LostRouteMessage::getType() const
{
    return TYPE;
}

/**
 * @brief Clone this message.
 *
 * @return a LostRouteMessage equal to this instance.
 */
TMessage* LostRouteMessage::clone() const
{
    return new LostRouteMessage(route);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *LostRouteMessage::query(const vector<string> *options) const {
    QueryResult *qr = new QueryResult("message");
    qr->insert("route", route.toString());
    return qr;            
}        

//----------------------------------------------------------------------------
//------------------------------< RouteChangedMaskMessage >--------------------------
//----------------------------------------------------------------------------

/**
 * @brief Create a RouteChangedMaskMessage.
 *
 * @param route the address for the route that changed the mask.
 */
RouteChangedMaskMessage::RouteChangedMaskMessage(const HypercubeMaskAddress &route) : route(route)
{
}

/**
 * @brief Get the address for the route that changed the mask.
 *
 * @param the address for the route that changed the mask.
 */
HypercubeMaskAddress RouteChangedMaskMessage::getRoute() const
{
    return route;
}

/** 
 * @brief Get the type of message.
 *
 * @return the type of message (ROUTE_CHANGED_MASK)
 */
string RouteChangedMaskMessage::getType() const
{
    return TYPE;
}

/**
 * @brief Clone this message.
 *
 * @return a RouteChangedMaskMessage equal to this instance.
 */
TMessage* RouteChangedMaskMessage::clone() const
{
    return new LostRouteMessage(route);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *RouteChangedMaskMessage::query(const vector<string> *options) const {
    QueryResult *qr = new QueryResult("message");
    qr->insert("route", route.toString());
    return qr;            
}        


//----------------------------------------------------------------------------
//----------------------------< CantConnectMessage >------------------------
//----------------------------------------------------------------------------
/**
 * @brief Create a CantConnectMessage.
 *
 * @param reason the reason why it can't connect.
 */
CantConnectMessage::CantConnectMessage(const string &reason) : reason(reason)
{
}

/**
 * @brief Get the reason why it can't connect.
 *
 * @return the reason why it can't connect.
 */
string CantConnectMessage::getReason() const
{
    return reason;
}

/** 
 * @brief Get the type of message.
 *
 * @return the type of message (CANT_CONNECT)
 */
string CantConnectMessage::getType() const
{
    return TYPE;
}

/**
 * @brief Clone this message.
 *
 * @return a CantConnectMessage equal to this instance.
 */
TMessage* CantConnectMessage::clone() const
{
    return new CantConnectMessage(reason);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *CantConnectMessage::query(const vector<string> *options) const {
    QueryResult *qr = new QueryResult("message");
    qr->insert("reason", reason);
    return qr;            
}        

//---------------------------------------------------------------------
//----------------------------< WaitMeMessage >------------------------
//---------------------------------------------------------------------
/**
 * @brief Create a WaitMeMessage.
 *
 * @param id the unique identification for the wait me request.
 */
WaitMeMessage::WaitMeMessage(long id) : id(id) 
{
}

/**
 * @brief Get the unique identification for the wait me request.
 *
 * @return the unique identification for the wait me request.
 */
long WaitMeMessage::getId() const
{
    return id;
}

/** 
 * @brief Get the type of message.
 *
 * @return the type of message (WAIT_ME)
 */
string WaitMeMessage::getType() const
{
    return TYPE;
}

/**
 * @brief Clone this message.
 *
 * @return a WaitMeMessage equal to this instance.
 */
TMessage* WaitMeMessage::clone() const
{
    return new WaitMeMessage(id);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *WaitMeMessage::query(const vector<string> *options) const {
    QueryResult *qr = new QueryResult("message");
    qr->insert("id", toStr(id));
    return qr;            
}        


//---------------------------------------------------------------------
//-------------------------< ReadyForDiscMessage >---------------------
//---------------------------------------------------------------------
/**
 * @brief Create a WaitMeMessage.
 *
 * @param id the unique identification matching the wait me id.
 */
ReadyForDiscMessage::ReadyForDiscMessage(long id) : id(id) 
{
}

/**
 * @brief Get the unique identification matching the wait me id.
 *
 * @return the unique identification matching the wait me id.
 */
long ReadyForDiscMessage::getId() const
{
    return id;
}

/** 
 * @brief Get the type of message.
 *
 * @return the type of message (READY_FOR_DISC)
 */
string ReadyForDiscMessage::getType() const
{
    return TYPE;
}

/**
 * @brief Clone this message.
 *
 * @return a ReadyForDiscMessage equal to this instance.
 */
TMessage* ReadyForDiscMessage::clone() const
{
    return new ReadyForDiscMessage(id);
}

/**
 * @brief Get information about this message.
 *
 * @param options options for the query, not used
 * @return a QueryResult with information about this message.
 */
QueryResult *ReadyForDiscMessage::query(const vector<string> *options) const {
    QueryResult *qr = new QueryResult("message");
    qr->insert("id", toStr(id));
    return qr;            
}        

}
}

