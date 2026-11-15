/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include "../../include/Misc/Timer.h"

Timer::Timer() {
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void Timer::start() {
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void Timer::stop() {
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

    //Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void Timer::pause() {
    //If the timer is running and isn't already paused
    if (mStarted && !mPaused) {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}

void Timer::unpause() {
    //If the timer is running and paused
    if (mStarted && mPaused) {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 Timer::getTicks() const {
    //The actual timer time
    Uint32 time = 0;

    //If the timer is running
    if (mStarted) {
        //If the timer is paused
        if (mPaused) {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        } else {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool Timer::isStarted() const {
    //Timer is running and paused or unpaused
    return mStarted;
}

bool Timer::isPaused() const {
    //Timer is running and paused
    return mPaused && mStarted;
}

float Timer::getTotalTime() const
{
    float totalTime = 0;

    if (mStarted) {
        if (mPaused) {
            totalTime = static_cast<float>(mPausedTicks);
        } else {
            totalTime = static_cast<float>(SDL_GetTicks()) - static_cast<float>(mStartTicks);
        }
    }

    return totalTime / 1000;
}