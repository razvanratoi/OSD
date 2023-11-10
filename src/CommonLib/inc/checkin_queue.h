#pragma once

C_HEADER_START
typedef struct _CHECKIN_QUEUE
{
    volatile BOOLEAN*       Array;

    DWORD                   NumberOfThreads;

    DWORD                   BufferSize;
} CHECKIN_QUEUE, * PCHECKIN_QUEUE;

//******************************************************************************
// Function:     CheckinQueueInit
// Description:  Pre-Initializes a checkin_queue. No other CheckinQueue* function
//               can be used before this function is called. The function returns
//               the needed Buffer size for the internal queue.
// Returns:      DWORD - The size(in bytes) of the buffer needed for the internal
//               queue.
// Parameter:    OUT       PCHECKIN_QUEUE      Queue
// Parameter:    IN        DWORD               NumberOfThreads
//******************************************************************************
DWORD
CheckinQueuePreInit(
    OUT       PCHECKIN_QUEUE      Queue,
    IN        DWORD               NumberOfThreads
);

//******************************************************************************
// Function:     CheckinQueueInit
// Description:  Initializes a checkin_queue.  The checking_queue must be pre-
//               initialized before by calling CheckinQueuePreInit.
//               An already allocated array with exactly BufferSize bytes,
//               returned by the CheckinQueuePreInit function, has to be passed
//               to this function.
// Returns:      void
// Parameter:    INOUT       PCHECKIN_QUEUE      Queue
// Parameter:    IN          PBYTE               QueueBuffer
//******************************************************************************
void
CheckinQueueInit(
    INOUT     PCHECKIN_QUEUE      Queue,
    IN        PBYTE               QueueBuffer
);

//******************************************************************************
// Function:     CheckinQueueUninit
// Description:  Un-initializes the CheckinQueue structure, the memory allocated
//               for the queue must be de-allocated outside of this function
//               as from here the reference to the allocated memory zone is lost.
// Returns:      void
// Parameter:    INOUT       PCHECKIN_QUEUE      Queue
//******************************************************************************
void
CheckinQueueUninit(
    INOUT     PCHECKIN_QUEUE      Queue
);

//******************************************************************************
// Function:     CheckinQueueMarkPresence
// Description:  Waits a very little until it can mark a single presence in the
//               queue. The marking is done atomically.
// Returns:      void
// Parameter:    INOUT       PCHECKIN_QUEUE       Queue
//******************************************************************************
void
CheckinQueueMarkPresence(
    INOUT       PCHECKIN_QUEUE       Queue
);

//******************************************************************************
// Function:     CheckinQueueWaitOn
// Description:  Spins until all threads or CPUs marked there presence inside
//               the queue or if WaitAll is false, it waits for
//               NumberOfCheckinsToWait presences. If this number is higher than
//               the length of the queue, it will wait until the full queue is
//               marked as present.
// Returns:      void
// Parameter:    IN          PCHECKIN_QUEUE      Queue
// Parameter:    IN          BOOLEAN             WaitAll
// Parameter:    IN_OPT      DWORD               NumberOfCheckinsToWait
//******************************************************************************
void
CheckinQueueWaitOn(
    IN          PCHECKIN_QUEUE      Queue,
    IN          BOOLEAN             WaitAll,
    IN_OPT      DWORD               NumberOfCheckinsToWait
);

C_HEADER_END
