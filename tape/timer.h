#ifndef TIMER_H
#define TIMER_H

#if __cplusplus <= 199711L

#include <time.h>

class timer {
private:
    clock_t start_time;
public:
    typedef double result_type;

    void start() {
        start_time = clock();
    }

    result_type stop() {
        return 1000000000. * ((clock() - start_time) / double(CLOCKS_PER_SEC));
    }
};

#else

#include <chrono>
class timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double stop() {
        auto stop_time = std::chrono::high_resolution_clock::now();
        return double(std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count());
    }
};

#endif

#endif
