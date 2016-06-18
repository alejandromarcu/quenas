#include <vector>
#include "Data.h"

namespace simulator {
    namespace dataUnit {

using namespace std;
        
/**
 * @brief Creates a data unit containing the specified string.
 *
 * @param str the content of this data unit
 */
Data::Data(const string& str)
{
    copy(str.begin(), str.end(), back_inserter(data));
}

/**
 * @brief Creates a data unit containing the specified byte array.
 *
 * @param data the content of this data unit
 */
Data::Data(const VB& data) : TData(data) 
{
}


/**
 * @brief Dumps this data unit to an iterator.
 * 
 * This data unit doesn't contain any header, so just the data is
 * dumped to the iterator
 *
 * @param it the iterator where the data unit is dumped.
 */
void Data::dumpTo(BackItVB it) const 
{
    copy(data.begin(), data.end(), it);
};        

/**
 * @brief Returns a string representation of the data contained in this data unit.
 *
 * @return a string representation of the data contained in this data unit.
 */
string Data::toString() const {
    string str;
    copy(data.begin(), data.end(), back_inserter(str));
    return str;
}




}
}
