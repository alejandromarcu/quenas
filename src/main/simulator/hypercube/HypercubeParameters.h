#ifndef _HYPERCUBEPARAMETERS_H_
#define _HYPERCUBEPARAMETERS_H_

#include "Units.h"

namespace simulator {
    namespace hypercube {

/**
 * @brief Class to hold parameters for hypercube protocol.
 */
class HypercubeParameters {
    public:
        const static Time WAIT_PAN_TIMEOUT;
        const static Time LISTEN_HB_TIMEOUT;
        const static Time WAIT_SAN_TIMEOUT;
        const static Time WAIT_PAP_TIMEOUT;
        const static Time WAIT_PANC_TIMEOUT;
        const static Time WAIT_WAITME_TIMEOUT;
        const static Time HEARD_BIT_PERIOD;
        const static Time RENDEZ_VOUS_LOOKUP_TABLE_RECEIVED_TIMEOUT;
        const static Time RENDEZ_VOUS_CLIENT_CACHE_CLEANING_PERIOD;
        const static Time ROUTING_TABLE_ENTRY_CLEAR_PERIOD;
        const static Time ROUTING_TABLE_BITMAP_CLEAR_PERIOD;

        const static int NEIGHBOURS_BEFORE_PARENT;
};

}}

#endif
