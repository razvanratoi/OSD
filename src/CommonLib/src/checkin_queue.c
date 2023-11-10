#include "common_lib.h"
#include "lock_common.h"
#include "checkin_queue.h"

#ifndef _COMMONLIB_NO_LOCKS_

DWORD
CheckinQueuePreInit(
    OUT       PCHECKIN_QUEUE      Queue,
    IN        DWORD               NumberOfThreads
)
{
    ASSERT(NULL != Queue);
    ASSERT(0 < NumberOfThreads);

    memzero(Queue, sizeof(CHECKIN_QUEUE));

    Queue->NumberOfThreads = NumberOfThreads;

    Queue->BufferSize = NumberOfThreads * sizeof(BOOLEAN);

    return Queue->BufferSize;
}

void
CheckinQueueInit(
    INOUT     PCHECKIN_QUEUE      Queue,
    IN        PBYTE               QueueBuffer
)
{
    ASSERT(NULL != Queue);
    ASSERT(NULL != QueueBuffer);
    ASSERT(0 < Queue->NumberOfThreads); // a validation in-case PreInit wasn't called

    Queue->Array = QueueBuffer;

    memset((void*)Queue->Array, FALSE, Queue->NumberOfThreads);
}

void
CheckinQueueUninit(
    INOUT     PCHECKIN_QUEUE      Queue
)
{
    ASSERT(NULL != Queue);

    memzero(Queue, sizeof(CHECKIN_QUEUE));
}

void
CheckinQueueMarkPresence(
    INOUT       PCHECKIN_QUEUE       Queue
)
{
    ASSERT(NULL != Queue);
    ASSERT(0 < Queue->NumberOfThreads);

    // mark here that thread or cpu arrived where it should be after creation
    // little busy wait here, it should be basically nothing or minimum waiting time (atomic operation)
    for (DWORD i = 0; i < Queue->NumberOfThreads; i++)
    {
        if (!_InterlockedCompareExchange8(&Queue->Array[i], TRUE, FALSE))
        {
            // marked my slot, the index of the slot is not an issue
            break;
        }
    }
}


void
CheckinQueueWaitOn(
    IN          PCHECKIN_QUEUE      Queue,
    IN          BOOLEAN             WaitAll,
    IN_OPT      DWORD               NumberOfCheckinsToWait
)
{
    ASSERT(NULL != Queue);
    ASSERT(0 < Queue->NumberOfThreads);

    INTR_STATE oldState;
    BOOLEAN busyWait = TRUE;
    DWORD numberOfThreads = (WaitAll) ? Queue->NumberOfThreads : NumberOfCheckinsToWait;

    // disable interrupts, as we shouldn't been interrupted.
    oldState = CpuIntrDisable();

    // don't let the main thread or cpu advance until every thread signaled
    // that it arrived before the blocking sequence.
    while (busyWait)
    {
        BOOLEAN semaphoreGreen = TRUE;
        for (DWORD i = 0; i < numberOfThreads; i++)
        {
            if (!_InterlockedCompareExchange8(&Queue->Array[i], TRUE, TRUE))
            {
                semaphoreGreen = FALSE;
            }
            _mm_pause();
        }
        if (semaphoreGreen) busyWait = FALSE;
    }

    // set back the interrupt state
    CpuIntrSetState(oldState);
}

#endif // _COMMONLIB_NO_LOCKS_