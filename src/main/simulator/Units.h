#ifndef _SIMULATOR_TIME_H_
#define _SIMULATOR_TIME_H_

#include <string>
#include "common.h"

namespace simulator {
    
using namespace std;
    
/**
 * @brief Represents an instant or difference of time.
 * The minimum unit is a nano second.
 */    
class Time {
    public:
        const static long long NANOSEC;
        const static long long MICROSEC;
        const static long long MILISEC;        
        const static long long SEC;
        const static long long MIN;
        const static long long HOUR;

        Time();        
        Time(long long time);
        Time(string t, long long defaultMultiplier = SEC);
        
        long long getValue() const;
        string toString() const;
        string toString(long long unit) const;

        Time& operator+=(Time t);
        bool operator>(const Time &t);
        bool operator>=(const Time &t);
        bool operator<(const Time &t);
        bool operator<=(const Time &t);
        bool operator==(const Time &t);
        bool operator!=(const Time &t);
   
    private:
        /// Internal representation in nano seconds
        long long time;
};
  
/** 
 * @brief Represents the bandwidht of a connection
 */
class Bandwidth {
    public:
        const static long BPS;
        const static long KBPS;
        const static long MBPS;        
        const static long GBPS;                
        
        Bandwidth();
        Bandwidth(long bpsValue);
        Bandwidth(string bw, long defaultMultiplier = BPS);
        
        long bpsValue() const;        
        string toString() const;

    private:
        /// value in BPS
        long value;

};

}
#endif
