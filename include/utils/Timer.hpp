#ifndef TIMER_H
#define TIMER_H

#ifdef TIMER_ENABLE_LOG
#include <stdio.h>
#endif

#include <chrono>
class Timer
{
private:
    decltype(std::chrono::high_resolution_clock::now()) startTime;

public:
    Timer()
    {
#ifdef TIMER_ENABLE_LOG
        printf("[Timer: Timer started]\n");
#endif
        startTime = std::chrono::high_resolution_clock::now();
    }
    double queryTime()
    {
        auto curTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durTime = curTime - startTime;
        double dur_ms = durTime.count() * 1e3;
#ifdef TIMER_ENABLE_LOG
        printf("[Timer: Current Time: %fms]\n", dur_ms);
#endif
        return dur_ms;
    }
    ~Timer()
    {
        auto curTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durTime = curTime - startTime;
#ifdef TIMER_ENABLE_LOG
        printf("[Timer: Spent %fms in current scope]\n", durTime.count() * 1e3);
#endif
    }
};

#endif // ifndef TIMER_H