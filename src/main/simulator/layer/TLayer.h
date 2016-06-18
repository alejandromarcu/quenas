#ifndef _TLAYER_H_
#define _TLAYER_H_

#include "common.h"

namespace simulator {
    
class TNode;
    
namespace layer {

/**
 * @brief Base class for all the layers.
 */
class TLayer {
    public:
        TLayer(TNode *node) : node(node) {};
        
        virtual ~TLayer() {};
        TNode *getNode() { return node; };
        TNode *getNode() const { return node; };
        
    private:
        /// Node where the layer runs
        TNode *node;
};

}
}
#endif
