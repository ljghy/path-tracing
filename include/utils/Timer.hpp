#ifndef TIMER_H
#define TIMER_H

#ifdef TIMER_ENABLE_LOG
#include <cstdio>
#endif

#include <chrono>
class Timer
{
private:
    decltype(std::chrono::high_resolution_clock::now()) startTime;
    decltype(std::chrono::high_resolution_clock::now()) lastTime;

public:
    Timer()
    {
#ifdef TIMER_ENABLE_LOG
        printf("[Timer: Timer started]\n");
#endif
        lastTime = startTime = std::chrono::high_resolution_clock::now();
    }

    double queryTime()
    {
        auto curTime = lastTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durTime = curTime - startTime;
        double dur_ms = durTime.count() * 1e3;
#ifdef TIMER_ENABLE_LOG
        printf("[Timer: Current time: %fms]\n", dur_ms);
#endif
        return dur_ms;
    }
    double deltaTime()
    {
        auto curTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durTime = curTime - lastTime;
        lastTime = curTime;
        double dur_ms = durTime.count() * 1e3;
#ifdef TIMER_ENABLE_LOG
        printf("[Timer: Past time since last query: %fms]\n", dur_ms);
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