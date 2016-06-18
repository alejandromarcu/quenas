#ifndef _DATA_H_
#define _DATA_H_

#include <vector>
#include "DataUnit.h"

namespace simulator {
    namespace dataUnit {

using namespace std;
        
/**
 * @brief A data unit used to communicate between application layers.
 *
 * It just contains data (no headers) that can be viewed as a byte array or
 * as a string.
 */        
class Data : public TData {
    public:
        Data(const string& str);
        Data(const VB& data);
        virtual void dumpTo(BackItVB it) const;
        string toString() const;
};



}
}

#endif
