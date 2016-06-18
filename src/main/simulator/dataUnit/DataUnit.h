#ifndef _DATAUNIT_H_
#define _DATAUNIT_H_

#include <vector>
#include <iterator>
#include "common.h"

namespace simulator {
    namespace dataUnit {

using namespace std;
        
/**
 * @brief Base class for all data units.
 *
 */        
class TDataUnit {
    protected:
        /// represents the data or payload that this data unit carries
        VB data;
        
    public:
        /**
         * @brief Empty constructor, does nothing
         */
        TDataUnit() {};
                
        /**
         * @brief Initializes the data field with a copy.
         *
         * @param data the data to initialize this data unit
         */
        TDataUnit(const VB &data) : data(data) {};
        
        /**
         * @brief Virtual Destructor. Does Nothing.
         */
        virtual ~TDataUnit() {};

        /**
         * @brief dumps the data unit to an iterator
         *
         * Inheriting classes must implement this method so that it dumps the
         * data unit header as well as the payload into the iterator.
         * 
         * @param it the iterator where the data unit is dumped.
         */
        virtual void dumpTo(BackItVB it) const = 0;
        
        /**
         * @brief Return the payload for this data unit.
         *
         * @return the payload for this data unit.
         */
        const VB& getPayload() const {
            return data;
        }
        
};


/**
 * @brief Base class for the data units that transport data between 
 * application layers
 */
class TData : public TDataUnit {
    public:
        /**
         * @brief Empty constructor, does nothing
         */
        TData() {};

        /**
         * @brief Initializes the data field with a copy.
         *
         * @param data the data to initialize this data unit
         */
        TData(const VB& data) : TDataUnit(data) {};
};

/**
 * @brief Base class for the data units that transport data between 
 * transport layers
 */
class TSegment : public TDataUnit {
    public:
        /**
         * @brief Empty constructor, does nothing
         */
        TSegment() {};

        /**
         * @brief Initializes the data field with a copy.
         *
         * @param data the data to initialize this data unit
         */
        TSegment(const VB& data) : TDataUnit(data) {};
};

/**
 * @brief Base class for the data units that transport data between 
 * networks layers
 */
class TPacket : public TDataUnit {
    public:
        /**
         * @brief Empty constructor, does nothing
         */
        TPacket() {};
                
        /**
         * @brief Initializes the data field with a copy.
         *
         * @param data the data to initialize this data unit
         */
        TPacket(const VB& data) : TDataUnit(data) {};
};

/**
 * @brief Base class for the data units that transport data between 
 * data link layers
 */
class TFrame : public TDataUnit {
    public:
        /**
         * @brief Empty constructor, does nothing
         */
        TFrame() {};

        /**
         * @brief Initializes the data field with a copy.
         *
         * @param data the data to initialize this data unit
         */
        TFrame(const VB& data) : TDataUnit(data) {};
};

/**
 * @brief Base class for the data units that transport data between 
 * physical layers
 */
class TBitStream : public TDataUnit {
    public:
        /**
         * @brief Empty constructor, does nothing
         */
        TBitStream() {};

        /**
         * @brief Initializes the data field with a copy.
         *
         * @param data the data to initialize this data unit
         */
        TBitStream(const VB& data) : TDataUnit(data) {};
        
        /**
         * @brief Get the length of this bit stream in bytes
         *
         * @return the length of this bit stream in bytes
         */
        int getLength() const
        {
            return data.size();
        }
};


}
}

#endif
