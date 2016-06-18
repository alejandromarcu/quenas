#ifndef _TNETWORK_H_
#define _TNETWORK_H_

#include "TNode.h"

namespace simulator {
    
/**
 * @brief Base class for networks.
 */
class TNetwork : public TCommandRunner {
    public:
        virtual ~TNetwork() {};
        virtual void addNode(TNode *node) = 0;
};

}

#endif
    
