#include "HypercubeParameters.h"

namespace simulator {
    namespace hypercube {
        
/// How long to wait for a Primary Address Notification Packet
const Time HypercubeParameters::WAIT_PAN_TIMEOUT(Time::MILISEC * 500);

/// How long to wait for all the nodes to send their Heard Bits
const Time HypercubeParameters::LISTEN_HB_TIMEOUT(Time::MILISEC * 500);

/// How long to wait for a Secundary Address Notification packet
const Time HypercubeParameters::WAIT_SAN_TIMEOUT(Time::MILISEC * 100);

/// How long to wait for a Primary Address Proposal packet
const Time HypercubeParameters::WAIT_PAP_TIMEOUT(Time::MILISEC * 100);

/// How long to wait for a Primary Address Notification Confirmation packet
const Time HypercubeParameters::WAIT_PANC_TIMEOUT(Time::MILISEC * 100);

/// How long to wait for a Wait Me message from other processes
const Time HypercubeParameters::WAIT_WAITME_TIMEOUT(Time::MILISEC * 10);

/// How often a Heard Bit must be sent.
const Time HypercubeParameters::HEARD_BIT_PERIOD(Time::MILISEC * 400);

/// How long to wait until a RV lookup table is confirmed to be received
const Time HypercubeParameters::RENDEZ_VOUS_LOOKUP_TABLE_RECEIVED_TIMEOUT(Time::MILISEC * 100);

/// How often to clean the RV client cache
const Time HypercubeParameters::RENDEZ_VOUS_CLIENT_CACHE_CLEANING_PERIOD(Time::SEC * 5);

/// How often to clean routing table entries
const Time HypercubeParameters::ROUTING_TABLE_ENTRY_CLEAR_PERIOD(Time::MIN * 5);

/// How often to clean routing table bitmap entries
const Time HypercubeParameters::ROUTING_TABLE_BITMAP_CLEAR_PERIOD(Time::MIN);

/// How many neighbours must be visited before going to the parent
const int HypercubeParameters::NEIGHBOURS_BEFORE_PARENT = 1;

}}
