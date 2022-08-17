#ifndef __ClockTime__h
#define __ClockTime__h

#include <bits/stdc++.h> 
#include <chrono> 

using namespace std::chrono::_V2;
/************************************************************
 * This class is used to control time spent by the program. 						
************************************************************/
class ClockTime {
    
private:
    system_clock::time_point START; /**< Records a specific time point in history. **/

public:
    /** Constructor. @param t A time point in history. **/
    ClockTime(const system_clock::time_point &t) : START(t) {};

    /** Returns the time point stored in START attribute. **/
    system_clock::time_point getStart() const { return START; }
    
    /** Changes the time point stored in START attribute. **/
    void setStart(const std::chrono::_V2::system_clock::time_point &t){ START = t; }

    /** Returns the current time point. **/
    static system_clock::time_point getTimeNow() { return std::chrono::high_resolution_clock::now(); }
    
    /** Returns the time in seconds spent from START until now. **/
    double getTimeInSecFromStart() const { 
        return 1e-9*(std::chrono::duration_cast<std::chrono::nanoseconds>(getTimeNow() - getStart()).count()); 
    }
    
};

#endif