#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

#include <vector>
#include "DataUnit.h"
#include "common.h"
#include "MACAddress.h"

namespace simulator {
    namespace dataUnit {

using namespace std;
        
/**
 * @brief A trivial bit stream used to communicate between physical layers.
 */        
class BitStream : public TBitStream {
    public:
        /**
         * @brief Create a Bit Stream that encapsulates a frame. It doesn't
         * add any header data.
         */
        BitStream(const TFrame &frame) 
        {
            frame.dumpTo(back_inserter(data));
        }
        
        /**
         * @brief Dumps this bitStream to an iterator. It doesn't dump
         * any aditional headers.
         *
         * @param it the back iterator where the data is inserted.
         */
        virtual void dumpTo(BackItVB it) const 
        {
            copy(data.begin(), data.end(), it);
        };        
};


}
}

#endif
