#include "boot.h"
#include "carthandle.h"
#include "CIC6105.h"
#include "idle.h"
#include "stack.h"
#include "libu64/stackcheck.h"
#include "z64thread.h"

StackEntry sBootStackInfo;
OSThread sIdleThread;
STACK(sIdleStack, 0x400);
StackEntry sIdleStackInfo;
STACK(sBootStack, BOOT_STACK_SIZE);

void bootproc(void) {
    StackCheck_Init(&sBootStackInfo, sBootStack, STACK_TOP(sBootStack), 0, -1, "boot");
    osMemSize = osGetMemSize();
    CIC6105_Init();
    osInitialize();
    osUnmapTLBAll();
    gCartHandle = osCartRomInit();
    StackCheck_Init(&sIdleStackInfo, sIdleStack, STACK_TOP(sIdleStack), 0, 0x100, "idle");
    osCreateThread(&sIdleThread, Z_THREAD_ID_IDLE, Idle_ThreadEntry, NULL, STACK_TOP(sIdleStack), Z_PRIORITY_IDLE);
    osStartThread(&sIdleThread);
}
