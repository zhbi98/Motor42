#include "MultiTimer.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

// Platform-specific function to get current ticks (milliseconds)
uint64_t getPlatformTicks() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000LL + now.tv_usec / 1000;
}

// Callback functions for the timers
void timerCallback1(MultiTimer* timer, void* userData) {
    printf("Timer 1 fired at %lu ms\n", getPlatformTicks());
    multiTimerStart(timer, 500, timerCallback1, NULL); // Restart timer
}

void timerCallback2(MultiTimer* timer, void* userData) {
    printf("Timer 2 fired at %lu ms\n", getPlatformTicks());
    multiTimerStart(timer, 1000, timerCallback2, NULL); // Restart timer
}

void timerCallback3(MultiTimer* timer, void* userData) {
    printf("Timer 3 (one-shot) fired at %lu ms\n", getPlatformTicks());
}

void timerCallback4(MultiTimer* timer, void* userData) {
    printf("Timer 4 is stopping Timer 1 at %lu ms\n", getPlatformTicks());
    multiTimerStop((MultiTimer*)userData);
}

int main() {
    multiTimerInstall(getPlatformTicks);

    MultiTimer timer1, timer2, timer3, timer4;

    multiTimerStart(&timer1, 500, timerCallback1, NULL); // 500 ms repeating
    multiTimerStart(&timer2, 1000, timerCallback2, NULL); // 1000 ms repeating
    multiTimerStart(&timer3, 2000, timerCallback3, NULL); // 2000 ms one-shot
    multiTimerStart(&timer4, 3000, timerCallback4, &timer1); // 3000 ms, stops timer1

    // Main loop to simulate time passage and process timers
    while (1) {
        multiTimerYield();
        usleep(1000); // Sleep for 1 ms
    }

    return 0;
}