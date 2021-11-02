
//----------------------------------------
// BIOS header files
//----------------------------------------
#include <xdc/std.h>  						//mandatory - have to include first, for BIOS types
#include <ti/sysbios/BIOS.h> 				//mandatory - if you call APIs like BIOS_start()
#include <xdc/runtime/Log.h>				//needed for any Log_info() call
#include <xdc/cfg/global.h> 				//header file for statically defined objects/handles
//defined objects/handles
#include <ti/sysbios/knl/Task.h>			//Tasks
#include <ti/sysbios/knl/Semaphore.h>		//Semaphores
#include <ti/sysbios/knl/Queue.h> 			//Queue
#include <ti/sysbios/knl/Swi.h>				//Swi

//-----------------------------------------
// MSP430 Header Files
//-----------------------------------------
#include <driverlib.h>


//-----------------------------------------
// MSP430 MCLK frequency settings
// Used to set MCLK frequency
// Ratio = MCLK/FLLREF = 8192KHz/32KHz
//-----------------------------------------
#define MCLK_DESIRED_FREQUENCY_IN_KHZ  8000                            // 8MHz
#define MCLK_FLLREF_RATIO              MCLK_DESIRED_FREQUENCY_IN_KHZ / ( UCS_REFOCLK_FREQUENCY / 1024 )    // Ratio = 250

#define GPIO_ALL	GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3| \
					GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7

#define RED GPIO_PORT_P1, GPIO_PIN0		//Red_Led
#define GREEN GPIO_PORT_P4, GPIO_PIN7	//Green_Led

#define MAINTAIN_CANAL_SPEEP_TIME 6000 //3 sec
#define VSG_NUM 3 //Number of Vessel Priorities (Effectively number of Vessel Tasks!)
#define MAX_NAME_LEN 15 //Maximum Length of Vessel Name String
#define MAP_POINTS 8 	// Num of points on the map

//Macro
#define redLedToggle(times) (ledToggle(RED,times))	// Red led
#define greenLedToggle(times) (ledToggle(GREEN,times)) // Green led


//ledToBlink options
#define LEDBLINK_GREEN 0x01 		//greend led
#define LEDBLINK_RED 0x02		//red led
#define LEDBLINK_BOTH 0x03		//both leds
#define GRADATION_BLINK 0x04	//leds gradation

//num of blinks
#define MAP2BLINKS 4
#define MAP4BLINKS 6
#define MAP6BLINKS 5
#define MAP8BLINKS 7

//define swi
#define MINUS_ONE (*(volatile signed int *))(-1);
//-----------------------------------------
// Prototypes
//-----------------------------------------

//Message Structure for the Queue communication between each vessel Task and ledSrvTask.
typedef struct InitQMsg{
	Queue_Elem	elem;
	UInt	eventBitmask;
	UInt	ledToBlink;
	UInt    numOfBlinks;
} InitQMsg;


//The hardware_init function
void hardware_init(void);

/*Function name: tsClockHandler
Description: timesout every 500 milliseconds and forcefully Yields the currently running Task.
Input: none.
Output: none.
Algorithm: yields the currently running Task */
void tsClockHandler(void);

//Handler function for all Vessel Tasks
void sailAway(UArg arg0, UArg arg1);

//Handler function for maintShip (Maintenance Ship)
void maintainCanal(UArg arg0, UArg arg1);

//-------------------------
//Led Toggling Functions
//-------------------------
/*Function name: ledToggle
Description: according to the value of ledToBlink, ledToggle function/MACRO invoke- RED or GREEN.
sending it only the value of msg field numOBlinks.
Input: led port, led pin and number of blinking time.
Output: none.
Algorithm: making the RED or GREEN led change thier current state*/
void ledToggle(uint8_t selectedPort, uint16_t selectedPins, int times);

/*Function name: bothLedToggle
Description: toggles BOTH GREEN and RED Leds at the same time.
Input: the number of blinking times.
Output: none.
Algorithm: making the RED and GREEN led turn on/off at the same time*/
void bothLedToggle(int times);

/*Function name: gradationLedToggle
Description: toggle the Red LED and GREEN LED with delay 1/2 a second, leds blink in gradation.
Input: number of blinking times.
Output: none.
Algorithm: Toggles Red/Green LEDs gradually. */
void gradationLedToggle(int times);

/*Function name: oppositePriorityBarrier
Description: set to vessels the opposite priority and release the vessels in this order.
Input:  pointer to current map point and vessel’s ID.
Output: void
Algorithm: change to the opposite priority and call to oppFifoBarrierShared function for handle the vessel’s release */
void oppositePriorityBarrier(UInt *mapPointIndex, UInt vesselId);

/*Function name: oppFifoBarrierShared
Description : generic function to serve oppositePriorityBarrier and fifoBarrier map points.
Input: pointer to current map point.
Output: void
Algorithm: update the counter about vassel arrived and pend on the relevate semaphore.
When the last vessels arrived, start the relevant clock */
void oppFifoBarrierShared(UInt *mapPointIndex);

/*Function name: genericPriBarrierClkHandler.
Description: When the clock is started in Opposite_Priority_Barrier,
the clock handler start run and release each vessel with his real priority.
Input: vessel’s argument.
Output: none.
Algorithm: if there is a vessel on the barrier release and set the real priority and wait for the next vessel to came.
If there is no more vessels in the Barrier, “stop” this periodic Clock*/
void genericPriBarrierClkHandler(UArg arg);

/*Function name: canalCross.
Description: function for crossing the canal, the same specification is true for map points 2 and 6
(with another NumOfBlinks).
Input: pointer to current map point and vessel's ID.
Output: none.
Algorithm: only one vessel can occupy the canal at given time,
the another should wait at the entry until the vessel in canal leaves with post event and blinks.
vessels wait in the Opposite Priority Barrier in the order of their opposite priority and wait with the real priority.*/
void canalCross(UInt *mapPointIndex, UInt vesselId);

/*Function name: fifoBarrier.
Description: FIFO barrier function for handling the barrier, the same specification is true for map points 3 and 7.
Input: pointer to current map point and vessel's ID.
Output: none.
Algorithm: all arriving vessels should wait in the order of their arrival (in their real priority), then when all vessels wait on the Barrier,
single the clock to start release them with this order, but with priority 1 .*/
void fifoBarrier(UInt *mapPointIndex, UInt vesselId);

/*Function name: genericFifoBarrierClkHandler.
Description: FIFO barrier clock function handler.
Input: clock argument.
Output: none.
Algorithm: the clock start running when all 3 vessels arrived to barrier,
sets vessel’s priority to 1 and release each vessel.*/
void genericFifoBarrierClkHandler(UArg arg);

/*Function name: unloadAndEnd.
Description: function for unload point and end point.
Input: pointer to current map point and vessel's ID .
Output: none.
Algorithm: update the numOfBlinks field on both Unload and End point.
At Unload post each vessel post event and blink given num of times, at End point in addition to blink,
remove each vessel message from queue and signals the maintenance ship to end its liveness loop (maintenanceLivenessLoop = TRUE).*/
void unloadAndEnd(UInt *mapPointIndex, UInt vesselId);


//----------------------
//temporary functions
//----------------------
/*Function name: semHandle
Description: return the relevant semaphore according the location of map point.
Input: index of map point.
Output: return the relevant semaphore handle [for map points 1,3,5,7].
Algorithm: the function get the index of map point and return the relevant semaphore. */
Semaphore_Handle semHandle(UInt);

/*Function name: clkHandle
Description : return the relevant clock (FIFO or Barrier) handler according the location of map point.
Input: index of map point.
Output: return the relevant clock handler [barrier opposite map point 1, barrier FIFO map point 3, barrier opposite map point 5, barrier FIFO map point 7] .
Algorithm: the function get the index of map point and return the relevant clock handler.*/
Clock_Handle clkHandle(UInt);

/*Function name: ledSrvTaskHandler
Description : handler function for ledSrvTask.
Input:  none.
Output: none.
Algorithm: highest priority task. Loop until there is no more msg in queue,
get and find the relevant bitmask from the message and handle it by calling the relevant LedToggle function-
(LEDBLINK_BOTH, LEDBLINK_RED, LEDBLINK_GREEN, GRADATION_BLINK).
Then go back and pend for event.*/
void ledSrvTaskHandler(void);


/*Function name: delay
Description: delay function.
Input: none.
Output: none.
Algorithm: consumes CPU Cycles for 1/2 second. */
void delay(void);

/*Function name: makeMaintanceLivenessLoopTrue
Description: swi function to check if all vessels finished with its trigger value and then post the swi function to simply set the maintance liveness loop variable to true.
Input: none.
Output: none.
Algorithm: Maintance liveness loop global variable set to true */
void makeMaintanceLivenessLoopTrue(UArg arg0, UArg arg1);

//Map_Points Names enums
typedef enum
{
	Opposite_Priority_Barrier, //map pont : 1+5
	Canal_Crossing, //map point : 2+6
	FIFO_Barrier, //map point : 3+7
	Unload_Point, // map point : 4
	End_Point // map point : 8
} Points_name;


typedef void (*mapFunctionsPointer)(UInt*, UInt);	//Array of pointers to functions

//-----------------------------------------
// Globals
//-----------------------------------------
volatile const UInt vesselPriorities[VSG_NUM] = { 5,3,2 };//Original Priorities
volatile const char vesselNames[VSG_NUM][MAX_NAME_LEN] = { "OilTanker", "CargoVessel", "MailVessel" }; //Vessels Names
volatile Task_Handle vesselHandles[VSG_NUM]; //Tasks handels per vessel tasks.
volatile const Points_name mapPoints[MAP_POINTS] = { Opposite_Priority_Barrier,Canal_Crossing,FIFO_Barrier,Unload_Point,Opposite_Priority_Barrier,Canal_Crossing,FIFO_Barrier,End_Point };
volatile Bool maintenanceLivenessLoop = FALSE;
volatile UInt vesselArriveToBarrierCount[MAP_POINTS];
volatile mapFunctionsPointer mapFunctions[MAP_POINTS]={oppositePriorityBarrier,canalCross,fifoBarrier, unloadAndEnd,oppositePriorityBarrier, canalCross, fifoBarrier, unloadAndEnd};
volatile UInt maintenanceLivenessLoopCounter = 0;

Swi_Handle SwiLiveLoop;
Swi_Params SwiLiveLoopParams;

//---------------------------------------------------------------------------
// main()
//---------------------------------------------------------------------------
void main(void)
{
	hardware_init();

	//making swi dynamic relative to the number of vessels in the cluster.
	Swi_Params_init(&SwiLiveLoopParams);
	SwiLiveLoopParams.priority = (-1);
	SwiLiveLoopParams.trigger = VSG_NUM;
	SwiLiveLoopParams.arg0 = 0;
	SwiLiveLoopParams.arg1 = 0;

	SwiLiveLoop = Swi_create((Swi_FuncPtr)makeMaintanceLivenessLoopTrue, &SwiLiveLoopParams, NULL);

	// start the BIOS
	BIOS_start();
}


//-----------------------------------------------------------------------------
// hardware_init()
//-----------------------------------------------------------------------------
void hardware_init(void)					//called by main
{
	// Disable the Watchdog Timer (important, as this is enabled by default)
	WDT_A_hold(WDT_A_BASE);

	// Set MCLK frequency to 8192 KHz
	// First, set DCO FLLREF to 32KHz = REF0
	UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
	// Second, Set Ratio and Desired MCLK Frequency = 8192KHz and initialize DCO
	UCS_initFLLSettle(MCLK_DESIRED_FREQUENCY_IN_KHZ, MCLK_FLLREF_RATIO);

	// Set GPIO ports to low-level outputs
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_ALL);
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_ALL);
	GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_ALL);
	GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_ALL);
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_ALL);
	GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_ALL);
	GPIO_setAsOutputPin(GPIO_PORT_PJ, GPIO_ALL);

	GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_ALL);
	GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_ALL);
}


void sailAway(UArg arg0, UArg arg1)
{
	UInt i ;
	InitQMsg message;
	vesselHandles[(UInt)arg0-1] = Task_self();

	message.eventBitmask = 0x01 << ((UInt)arg0-1);
	message.ledToBlink = (UInt)arg1;
	message.numOfBlinks = 0;
	Queue_put(ledSpecQ, (Queue_Elem *)&message);

	Task_setEnv(Task_self(), (Ptr)&message);
	Log_info1("Vessel %s - Started Sailing...", (IArg)vesselNames[(UInt)arg0-1]);

	for (i = 1; i <= MAP_POINTS; i++)
		mapFunctions[i-1](&i, (UInt)arg0);

}

void delay(void)
{
	// ~1/2 second delay providing Clock Module is set to tick every 500 microseconds
	// (i.e every 1/2 milisecond, rather than every 1 milisecond).
	__delay_cycles(1024000);
}

void tsClockHandler(void)
{
	Task_yield();
}


void ledToggle(uint8_t selectedPort, uint16_t selectedPins, int times)
{
	int i;
	for (i = 0; i < times*2; i++)
	{
		delay();
		GPIO_toggleOutputOnPin(selectedPort,selectedPins);
		delay();
	}
}

void bothLedToggle(int times)
{
	int i;
	for (i = 0; i < times*2; i++)
	{
		delay();
		GPIO_toggleOutputOnPin(RED);
		GPIO_toggleOutputOnPin(GREEN);
		delay();
	}
}

void gradationLedToggle(int times)
{
	int i;
	for (i = 0; i < times*2; i++)
	{
		delay();
		GPIO_toggleOutputOnPin(RED);
		delay();
		delay();
		GPIO_toggleOutputOnPin(GREEN);
		delay();
	}
}

void maintainCanal(UArg arg0, UArg arg1)
{
	InitQMsg message;
	message.eventBitmask = 0x01 << ((UInt)arg0 - 1);
	message.ledToBlink = GRADATION_BLINK;
	message.numOfBlinks = 2;
	Queue_put(ledSpecQ, (Queue_Elem*)&message);

	while (!maintenanceLivenessLoop)
	{

		Task_sleep(MAINTAIN_CANAL_SPEEP_TIME);
		Event_post(vesselReqEvt, message.eventBitmask);
	}

	//when maintenanceLivenessLoop is true
	Swi_delete(&SwiLiveLoop);
	Queue_get(ledSpecQ);
	Event_post(vesselReqEvt, message.eventBitmask);
	Clock_stop(timeSharingClk);
	Log_info0("maintainCanal:: Task Terminated");
}


void oppositePriorityBarrier(UInt *mapPointIndex, UInt vesselId) {
	Task_setPri(vesselHandles[vesselId-1], vesselPriorities[VSG_NUM - vesselId]); //set opposite priority
	Log_info3("Vessel %s entered map point %d; Priority = %d", (IArg)vesselNames[vesselId-1],*mapPointIndex,Task_getPri(vesselHandles[vesselId-1]));

	oppFifoBarrierShared(mapPointIndex);
}

void oppFifoBarrierShared(UInt *mapPointIndex ){
	if (++vesselArriveToBarrierCount[*mapPointIndex-1] == VSG_NUM) {
		Clock_start(clkHandle(*mapPointIndex));
	}
	Semaphore_pend(semHandle(*mapPointIndex), BIOS_WAIT_FOREVER);
}

void genericPriBarrierClkHandler(UArg arg)
{
	if (vesselArriveToBarrierCount[(UInt)arg-1] == 0)
		Clock_stop(clkHandle((UInt)arg));
	else
	{
		Semaphore_post(semHandle((UInt)arg));
		Task_setPri(vesselHandles[vesselArriveToBarrierCount[(UInt)arg-1]-1], vesselPriorities[vesselArriveToBarrierCount[(UInt)arg-1]-1]); //set original priority
		vesselArriveToBarrierCount[(UInt)arg-1]--;
	}
}

void canalCross(UInt *mapPointIndex, UInt vesselId)
{
	Log_info3("Vessel %s entered map point %d; Priority = %d", (IArg)vesselNames[vesselId-1],*mapPointIndex,Task_getPri(vesselHandles[vesselId-1]));

	IArg canalGateKey;
	InitQMsg* message = (InitQMsg*)Task_getEnv(Task_self()); // cast because pointer returned is of type Ptr which is void* in TI-RTOS
	switch (*mapPointIndex)
	{
	case 2:
		canalGateKey = GateMutexPri_enter(gateMutexPri0);
		message->numOfBlinks = MAP2BLINKS;
		Event_post(vesselReqEvt, message->eventBitmask);
		delay();
		GateMutexPri_leave(gateMutexPri0,canalGateKey);
		break;
	case 6:
		canalGateKey = GateMutexPri_enter(gateMutexPri1);
		message->numOfBlinks =MAP6BLINKS;
		Event_post(vesselReqEvt, message->eventBitmask);
		delay();
		GateMutexPri_leave(gateMutexPri1,canalGateKey);
		break;
	default: Log_info0("canalCross :: - Abnormal Behavior ");
	}
}


void genericFifoBarrierClkHandler(UArg arg)
{
	int i;
	for(i=0; i<VSG_NUM; i++)
		Task_setPri(vesselHandles[i], 1);
	for(i=0; i<VSG_NUM; i++)
		Semaphore_post(semHandle((UInt)arg));
	vesselArriveToBarrierCount[(UInt)arg-1] = 0;
}


void fifoBarrier(UInt *mapPointIndex, UInt vesselId) {
	Log_info3("Vessel %s entered map point %d; Priority = %d", (IArg)vesselNames[vesselId-1],*mapPointIndex,Task_getPri(vesselHandles[vesselId-1]));
	oppFifoBarrierShared(mapPointIndex);
}

void unloadAndEnd(UInt *mapPointIndex, UInt vesselId)
{
	Log_info3("Vessel %s entered map point %d; Priority = %d", (IArg)vesselNames[vesselId-1],*mapPointIndex,Task_getPri(vesselHandles[vesselId-1]));

	InitQMsg *message;

	switch (mapPoints[*mapPointIndex-1])
	{
	case Unload_Point: //unload
		message = (InitQMsg*)Task_getEnv(Task_self()); // cast because pointer returned is of type Ptr which is void* in TI-RTOS
		message->numOfBlinks = MAP4BLINKS;
		Event_post(vesselReqEvt, message->eventBitmask);
		break;

	case End_Point: //end
		message = (InitQMsg*)Task_getEnv(Task_self()); // cast because pointer returned is of type Ptr which is void* in TI-RTOS
		message->numOfBlinks = MAP8BLINKS;
		Event_post(vesselReqEvt, message->eventBitmask);

		// Mutex, this is for making the remove operation atomic. so no one will interrupt the task while removing its message from the queue
		Semaphore_pend(endPointMutex, BIOS_WAIT_FOREVER);
		Queue_remove((Queue_Elem*)message);
		Semaphore_post(endPointMutex);
		// End Mutex

		Log_info1("Vessel %s Task Terminated", (IArg)vesselNames[vesselId-1]);
/*		if(++maintenanceLivenessLoopCounter == VSG_NUM )
		{
			maintenanceLivenessLoop = TRUE;
			break;
		}
*/
		Swi_dec(SwiLiveLoop);
		break;
	default: Log_info0("unloadAndEnd :: - Abnormal Behavior ");
	}

}

void ledSrvTaskHandler(void)
{
	UInt bmaskRes, bitMask;
	bitMask =~(0xFFFF << (VSG_NUM + 1));
	InitQMsg *message;
	while (TRUE)
	{
		bmaskRes = Event_pend(vesselReqEvt, Event_Id_NONE, bitMask, BIOS_WAIT_FOREVER);

		if(Queue_empty(ledSpecQ))//check if queue is empty
			break;

		message = (InitQMsg*)Queue_head(ledSpecQ);

		//Find the message that match the bmResults
		while(message->eventBitmask != bmaskRes)
			message = Queue_next((Queue_Elem *)message);

		//Blink the requiered pattern
		switch (message->ledToBlink)
		{
		case LEDBLINK_BOTH:
			bothLedToggle(message->numOfBlinks);
			break;
		case LEDBLINK_RED:
			redLedToggle(message->numOfBlinks);
			break;
		case LEDBLINK_GREEN:
			greenLedToggle(message->numOfBlinks);
			break;
		case GRADATION_BLINK:
			gradationLedToggle(message->numOfBlinks);
			break;
		default:
			break;
		}
	}
}


Clock_Handle clkHandle(UInt mapPointIndex) {
	switch (mapPointIndex)
	{
	case 1: return BarrierOppClk1;  //Clock :: map point 1
	case 3: return BarrierFIFOClk3; //Clock :: map point 3
	case 5: return BarrierOppClk5;  //Clock :: map point 5
	case 7: return BarrierFIFOClk7; //Clock :: map point 7
	default: Log_info1("clkHandle :: - Abnormal Behavior !! there is no Clock that mapping to mapPoint [%d]", mapPointIndex);
		return NULL;
	}
}

Semaphore_Handle semHandle(UInt mapPointIndex) {
	switch (mapPointIndex)
	{
	case 1: return BarrierOppSem1;  //Semaphore :: map point 1
	case 3: return BarrierFIFOSem3; //Semaphore :: map point 3
	case 5: return BarrierOppSem5;  //Semaphore :: map point 5
	case 7: return BarrierFIFOSem7; //Semaphore :: map point 7
	default: Log_info1("semHandle :: - Abnormal Behavior !! there is no Semaphore that mapping to mapPoint [%d]", mapPointIndex);
		return NULL;
	}
}

void makeMaintanceLivenessLoopTrue(UArg arg0, UArg arg1) {
	maintenanceLivenessLoop = TRUE;
}




