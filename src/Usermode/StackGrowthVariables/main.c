#include "common_lib.h"
#include "syscall_if.h"
#include "um_lib_helper.h"

// we turn all compiler optimizations off so we don't have any surprises
#pragma optimize("", off)

#define NO_OF_TIMES_TO_ALLOCATE_ALMOST_A_PAGE_OF_LOCAL_VARIABLES     10

#define VALUE_TO_PLACE_ON_LOCAL_VARIABLE                             0xAA

// explanation can be found in figure (I have no idea why Visual C compiler generates 0x30 bytes
// of 0xCC's for RTC at the start of the variable, but only 0x10 bytes at the end of it
// 0x70 (0x20 shadow stack + 0x8 RA + 0x8 RDI + 0x40 CC's)
#define MAX_LOCAL_VAR_ALLOC_LENGTH      (PAGE_SIZE - 0x70)

// the reason why we call multiple functions to allocate more than 1 page of variables is because
// if we want to allocate a PAGE_SIZE or more of local variables the Visual C compiler wants to call
// __chkstk which HAL currently doesn't support (__chkstk would actually do what we do in these functions,
// i.e. touch the local variables page by page)

//  STACK TOP
//  -----------------------------------------------------------------
//  |                                                               |
//  |       Shadow Space                                            |
//  |                                                               |
//  |                                                               |
//  -----------------------------------------------------------------   -0x20
//  |     Return Address                                            |
//  -----------------------------------------------------------------   -0x28
//  |     RDI                                                       |
//  -----------------------------------------------------------------   -0x30
//  |     RTC variable = 0xCC..CC                                   |
//  -----------------------------------------------------------------   -0x40
//  |     dummyVariable                                             |
//  -----------------------------------------------------------------   -0xFD0
//  |     RTC variable = 0xCC..CC                                   |
//  -----------------------------------------------------------------   -0x1000
static void _AllocateAlmostAPageOfLocalVariables(DWORD TimesToCall)
{
    BYTE dummyVariable[MAX_LOCAL_VAR_ALLOC_LENGTH];

    if (TimesToCall == 0) return;

    // on debug this isn't even needed because Visual C initializes the region
    // with CC's
    dummyVariable[0] = VALUE_TO_PLACE_ON_LOCAL_VARIABLE;

    _AllocateAlmostAPageOfLocalVariables(TimesToCall - 1);

    if (dummyVariable[0] != VALUE_TO_PLACE_ON_LOCAL_VARIABLE)
    {
        LOG_ERROR("Value placed on stack 0x%02x differs from value read 0x%02x\n",
            VALUE_TO_PLACE_ON_LOCAL_VARIABLE, dummyVariable[0]);
    }
}


STATUS
__main(
    DWORD       argc,
    char**      argv
)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    _AllocateAlmostAPageOfLocalVariables(NO_OF_TIMES_TO_ALLOCATE_ALMOST_A_PAGE_OF_LOCAL_VARIABLES);

    return STATUS_SUCCESS;
}
#pragma optimize("", on)
