#include "common_lib.h"
#include "syscall_if.h"
#include "um_lib_helper.h"

#define NO_OF_TIMES_TO_CALL_RECURSIVE_FUNCTIONS     10000

// we turn all compiler optimizations off so we don't have any surprises
#pragma optimize("", off)
static void _RecursiveFunction(DWORD TimesToCall)
{
    if (TimesToCall == 0) return;

    _RecursiveFunction(TimesToCall - 1);
}

STATUS
__main(
    DWORD       argc,
    char**      argv
)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    _RecursiveFunction(NO_OF_TIMES_TO_CALL_RECURSIVE_FUNCTIONS);

    return STATUS_SUCCESS;
}
#pragma optimize("", on)
