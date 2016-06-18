#include "Units.h"
#include <cmath>

namespace simulator {
const long long Time::NANOSEC = 1ll;
const long long Time::MICROSEC = 1000ll;
const long long Time::MILISEC = 1000000ll;
const long long Time::SEC = 1000000000ll;
const long long Time::MIN = 60000000000ll;
const long long Time::HOUR = 3600000000000ll;

const long Bandwidth::BPS = 1l;
const long Bandwidth::KBPS = 1000l;
const long Bandwidth::MBPS = 1000000l;
const long Bandwidth::GBPS = 1000000000l;



//----------------------------------------------------------------------
//-------------------------------< Time >-------------------------------
//----------------------------------------------------------------------      

/**
 * @brief Creates a Time object with time = 0.
 */
Time::Time() : time(0) 
{
}

/**
 * @brief Creates a Time object using the specified value.
 * The provided constants should be used, for example 25 * Time::MICROSEC.
 *
 * @param time the value for the time.  Provided constants should be used.
 */
Time::Time(long long time) : time(time) 
{
}

/**
 * @brief Create a Time object by parsing the string.
 * The string consist of a number, then zero or more spaces, and optionally
 * a multiplier, that can be one of the following: ns, us, ms, s, m, h.
 * For example "10", "24.5us", "1 h".
 * If no multiplier is specified, the defaultMultiplier is used.
 *
 * @brief t string to parse.
 * @brief defaultMultiplier unit of time used if none is provided in the string.
 */
Time::Time(string t, long long defaultMultiplier) {
    int pos = -1;
    for(int i=0; i < t.length(); i++) {
        if (!isdigit(t[i]) && t[i] != '.') {
            pos = i;
            break;
        }
    }
    
    if (pos < 0) {
        time = (long long) round(atof(t.c_str()) * defaultMultiplier);
    } else {

        string multip = trim(t.substr(pos));
        long long x;
        
        if (multip == "h") x = Time::HOUR;
        else if (multip == "m") x = Time::MIN;
        else if (multip == "s") x = Time::SEC;
        else if (multip == "ms") x = Time::MILISEC;
        else if (multip == "us") x = Time::MICROSEC;
        else if (multip == "ns") x = Time::NANOSEC;
        else throw invalid_argument("Unknown multiplier: " + multip);

        time = (long long) round(atof(t.substr(0, pos).c_str()) * x);
    }
    
}

/**
 * @brief Get a numeric representation for that time.
 *
 * @return a numeric representation for that time.
 */
long long Time::getValue() const {
    return time;
}

/**
 * @brief Get a string representation for that time.
 * The representation consist of a number followed by a time unit, using the
 * biggest time unit that makes the number greather than 1.
 * For example, 1.25 us, 990 ms.
 *
 * @return a string representation for that time.
 */
string Time::toString() const {
    if (time < Time::MICROSEC) return toStr((double) time / Time::NANOSEC) + " ns";
    if (time < Time::MILISEC) return toStr((double) time / Time::MICROSEC) + " us";
    if (time < Time::SEC) return toStr((double) time / Time::MILISEC) + " ms";
    if (time < Time::MIN) return toStr((double) time / Time::SEC) + " s";
    if (time < Time::HOUR) return toStr((double) time / Time::MIN) + " m";
    return toStr((double) time / Time::HOUR) + " h";
}

/**
 * @brief Get a string representation for that time.
 * The representation uses the specified unit.
 *
 * @param unit the desired unit, for example Time::SEC
 * @return a string representation for that time.
 */
string Time::toString(long long unit) const {
    if (unit == Time::NANOSEC) return toStr((double) time / Time::NANOSEC) + " ns";
    if (unit == Time::MICROSEC) return toStr((double) time / Time::MICROSEC) + " us";
    if (unit == Time::MILISEC) return toStr((double) time / Time::MILISEC) + " ms";
    if (unit == Time::SEC) return toStr((double) time / Time::SEC) + " s";
    if (unit == Time::MIN) return toStr((double) time / Time::MIN) + " m";
    if (unit == Time::HOUR) return toStr((double) time / Time::HOUR) + " h";

    throw invalid_argument("Bad unit: " + unit);
}

/**
 * @brief Add a time interval.
 *
 * @param t the time interval to add.
 * @return this object.
 */
Time& Time::operator+=(Time t) 
{
    time += t.getValue();
    return *this;
}

/**
 * @brief Sum two times.
 *
 * @brief t1 time to sum.
 * @brief t2 time to sum.
 * @return a time object that is the sum of both times.
 */
Time operator+(Time t1, Time t2) 
{
    Time result(t1);
    return t1 += t2;
}

/**
 * @brief Compares this time with another one.
 *
 * @param t the time to compare.
 * @return true if this time is later than t.
 */
bool Time::operator>(const Time &t)
{
    return this->getValue() > t.getValue();
}

/**
 * @brief Compares this time with another one.
 *
 * @param t the time to compare.
 * @return true if this time is equal or later than t.
 */
bool Time::operator>=(const Time &t)
{
    return this->getValue() >= t.getValue();
}

/**
 * @brief Compares this time with another one.
 *
 * @param t the time to compare.
 * @return true if this time is earlier than t.
 */
bool Time::operator<(const Time &t)
{
    return this->getValue() < t.getValue();
}

/**
 * @brief Compares this time with another one.
 *
 * @param t the time to compare.
 * @return true if this time is equal or earlier than t.
 */
bool Time::operator<=(const Time &t)
{
    return this->getValue() <= t.getValue();
}

/**
 * @brief Compares this time with another one.
 *
 * @param t the time to compare.
 * @return true if this time is equal to t.
 */
bool Time::operator==(const Time &t)
{
    return this->getValue() == t.getValue();
}

/**
 * @brief Compares this time with another one.
 *
 * @param t the time to compare.
 * @return true if this time is different of t.
 */
bool Time::operator!=(const Time &t)
{
    return this->getValue() != t.getValue();
}

//----------------------------------------------------------------------
//-----------------------------< Bandwidth >----------------------------
//----------------------------------------------------------------------      

/**
 * @brief Create a Bandwith with default value of 0.
 */
Bandwidth::Bandwidth() : value(0)
{
}

/**
 * @brief Create a Bandwidth.
 *
 * @param bpsValue value of the bandwidth.  Use provided constants
 */
Bandwidth::Bandwidth(long bpsValue) : value(bpsValue)
{
}

/**
 * @brief Create a Bandwidth.
 *
 * @param bw string representation of the value.
 * @param defaultMultiplier multiplier to use if no provided in bw,
 */
Bandwidth::Bandwidth(string bw, long defaultMultiplier) {
    int pos = -1;
    for(int i=0; i < bw.length(); i++) {
        if (!isdigit(bw[i]) && bw[i] != '.') {
            pos = i;
            break;
        }
    }
    
    if (pos < 0) {
        value = (long) round(atof(bw.c_str()) * defaultMultiplier);
    } else {

        string multip = trim(bw.substr(pos));
        long x;
        
        if (multip == "Gbps") x = Bandwidth::GBPS;
        else if (multip == "Mbps") x = Bandwidth::MBPS;
        else if (multip == "Kbps") x = Bandwidth::KBPS;
        else if (multip == "bps") x = Bandwidth::BPS;
        else throw invalid_argument("Unknown multiplier: " + multip);

        value = (long) round(atof(bw.substr(0, pos).c_str()) * x);
    }
    
}

/**
 * @brief Get the value in bps.
 *
 * @return the value in bps.
 */
long Bandwidth::bpsValue() const
{
    return value;
}
   
/**
 * @brief Get a string representation of the bandwidth.
 *
 * @return a string representation of the bandwidth.
 */
string Bandwidth::toString() const
{
    if (value < Bandwidth::KBPS) return toStr((double) value / Bandwidth::BPS) + " bps";
    if (value < Bandwidth::MBPS) return toStr((double) value / Bandwidth::KBPS) + " Kbps";
    if (value < Bandwidth::GBPS) return toStr((double) value / Bandwidth::MBPS) + " Mbps";
    return toStr((double) value / Bandwidth::GBPS) + " Gbps";

}


}
