
#ifndef SDL2_3D_ENGINE_LTIMER_H
#define SDL2_3D_ENGINE_LTIMER_H


#include <SDL2/SDL_timer.h>

class Timer {
public:
    Timer();

    void start();
    void stop();
    void pause();
    void unpause();

    [[nodiscard]] Uint32 getTicks() const;
    [[nodiscard]] bool isStarted() const;
    [[nodiscard]] bool isPaused() const;
    [[nodiscard]] float getTotalTime() const;

private:
    //The clock time when the timer started
    Uint32 mStartTicks;
    //The ticks stored when the timer was paused
    Uint32 mPausedTicks;
    //The timer status
    bool mPaused;
    bool mStarted;
};


#endif //SDL2_3D_ENGINE_LTIMER_H
