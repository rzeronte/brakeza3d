//
// Created by darkhead on 16/9/19.
//

#include "../../headers/Recastnavigation/RecastBuildContext.h"
#include "../../headers/Render/Logging.h"
#include <stdio.h>
#include <string.h>


RecastBuildContext::RecastBuildContext() :
        m_messageCount(0),
        m_textPoolSize(0)
{
    memset(m_messages, 0, sizeof(char*) * MAX_MESSAGES);

    resetTimers();
}

// Virtual functions for custom implementations.
void RecastBuildContext::doResetLog()
{
    m_messageCount = 0;
    m_textPoolSize = 0;
}

void RecastBuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
{
    if (!len) return;
    if (m_messageCount >= MAX_MESSAGES)
        return;
    char* dst = &m_textPool[m_textPoolSize];
    int n = TEXT_POOL_SIZE - m_textPoolSize;
    if (n < 2)
        return;
    char* cat = dst;
    char* text = dst+1;
    const int maxtext = n-1;
    // Store category
    *cat = (char)category;
    // Store message
    const int count = rcMin(len+1, maxtext);
    memcpy(text, msg, count);
    text[count-1] = '\0';
    m_textPoolSize += 1 + count;
    m_messages[m_messageCount++] = dst;

    Logging::getInstance()->Log(msg);
}

void RecastBuildContext::doResetTimers()
{
    for (int i = 0; i < RC_MAX_TIMERS; ++i)
        m_accTime[i] = -1;
}

void RecastBuildContext::doStartTimer(const rcTimerLabel label)
{
    m_startTime[label] = getPerfTime();
}

void RecastBuildContext::doStopTimer(const rcTimerLabel label)
{
    const TimeVal endTime = getPerfTime();
    const TimeVal deltaTime = endTime - m_startTime[label];
    if (m_accTime[label] == -1)
        m_accTime[label] = deltaTime;
    else
        m_accTime[label] += deltaTime;
}

int RecastBuildContext::doGetAccumulatedTime(rcTimerLabel label) {
    return getPerfTimeUsec(m_accTime[label]);
}

void RecastBuildContext::dumpLog(const char* format, ...)
{
    // Print header.
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
    printf("\n");

    // Print messages
    const int TAB_STOPS[4] = { 28, 36, 44, 52 };
    for (int i = 0; i < m_messageCount; ++i)
    {
        const char* msg = m_messages[i]+1;
        int n = 0;
        while (*msg)
        {
            if (*msg == '\t')
            {
                int count = 1;
                for (int j = 0; j < 4; ++j)
                {
                    if (n < TAB_STOPS[j])
                    {
                        count = TAB_STOPS[j] - n;
                        break;
                    }
                }
                while (--count)
                {
                    putchar(' ');
                    n++;
                }
            }
            else
            {
                putchar(*msg);
                n++;
            }
            msg++;
        }
        putchar('\n');
    }
}

int RecastBuildContext::getLogCount() const
{
    return m_messageCount;
}

const char* RecastBuildContext::getLogText(const int i) const
{
    return m_messages[i]+1;
}