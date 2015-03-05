/*
 *  Copyright (c) 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __KIS_LAZY_WAIT_CONDITION_H
#define __KIS_LAZY_WAIT_CONDITION_H

#include <QWaitCondition>

/**
 * This class is used for catching a particular condition met.
 * We call it "lazy", because the decision about the condition is made
 * by the waiting thread itself. The other thread, "wakingup" one,
 * just points the former one the moments of time when the condition
 * *might* have been satisfied. This creates some limitations for
 * the condition (see a note in the end of the text).
 *
 * Usage pattern:
 *
 * Waiting thread:
 *
 * KisLazyWaitCondition condition;
 * condition.initWainting();   // (1)
 * while(!checkSatisfied()) {
 *     condition.wait();
 * }
 * condition.endWaiting();     // (2)
 *
 *
 * Wakingup thread:
 *
 * if(checkMightSatisfied()) {
 *     condition.wakeAll();
 * }
 *
 * If the condition is met and reported, it is guaranteed that
 * all the threads, those are currently running between
 * lines (1) and (2) will be waken up and leave the loop.
 *
 * NOTE:
 * The condition checkSatisfied() must not change it's state, until
 * all the waiting threads leave the waiting loop. This requirement
 * must be guaranteed by the user of this class
 */

class KisLazyWaitCondition
{
public:
    KisLazyWaitCondition()
        : m_waitCounter(0),
          m_wakeupCounter(0)
    {
    }

    void initWaiting()
    {
        QMutexLocker locker(&m_mutex);
        if (!m_waitCounter) {
            m_wakeupCounter = 0;
        }

        m_waitCounter++;
    }

    void endWaiting()
    {
        QMutexLocker locker(&m_mutex);
        m_waitCounter--;
    }

    bool wait(unsigned long time = ULONG_MAX)
    {
        QMutexLocker locker(&m_mutex);
        bool result = true;
        if (!m_wakeupCounter) {
            result = m_condition.wait(&m_mutex, time);
        }
        if (result) {
            m_wakeupCounter--;
        }
        return result;
    }

    void wakeAll()
    {
        if (!m_waitCounter) {
            return;
        }

        QMutexLocker locker(&m_mutex);
        if (m_waitCounter) {
            m_wakeupCounter += m_waitCounter;
            m_condition.wakeAll();
        }
    }

    bool isSomeoneWaiting()
    {
        return m_waitCounter;
    }

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    volatile int m_waitCounter;
    int m_wakeupCounter;
};

#endif /* __KIS_LAZY_WAIT_CONDITION_H */
