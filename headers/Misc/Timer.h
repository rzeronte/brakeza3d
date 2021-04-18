
#ifndef SDL2_3D_ENGINE_LTIMER_H
#define SDL2_3D_ENGINE_LTIMER_H


#include <SDL2/SDL_types.h>
#include <SDL2/SDL_timer.h>

//The application time based timer
class Timer
{
public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    Uint32 getTicks();

    //Checks the status of the timer
    bool isStarted();
    bool isPaused();

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
