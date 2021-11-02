/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-A71
 */

#include <xdc/std.h>

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle BarrierOppClk1;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle BarrierOppSem1;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle CargoVessel;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle OilTankerVessel;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle mailVessel;

#include <ti/sysbios/knl/Queue.h>
extern const ti_sysbios_knl_Queue_Handle ledSpecQ;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle vesselReqEvt;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle timeSharingClk;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle ledSrvTask;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle BarrierFIFOClk3;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle BarrierFIFOSem3;

#include <ti/sysbios/gates/GateMutexPri.h>
extern const ti_sysbios_gates_GateMutexPri_Handle gateMutexPri0;

#include <ti/sysbios/gates/GateMutexPri.h>
extern const ti_sysbios_gates_GateMutexPri_Handle gateMutexPri1;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle BarrierOppClk5;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle BarrierOppSem5;

#include <ti/sysbios/knl/Clock.h>
extern const ti_sysbios_knl_Clock_Handle BarrierFIFOClk7;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle BarrierFIFOSem7;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle MaintenanceShip;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle endPointMutex;

#define TI_DRIVERS_WIFI_INCLUDED 0

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

#ifndef ti_sysbios_knl_Task__include
#ifndef __nested__
#define __nested__
#include <ti/sysbios/knl/Task.h>
#undef __nested__
#else
#include <ti/sysbios/knl/Task.h>
#endif
#endif

extern ti_sysbios_knl_Task_Struct TSK_idle;

