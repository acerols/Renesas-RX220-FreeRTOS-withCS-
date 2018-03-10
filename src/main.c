/* Hardware specific includes. */
#include "iodefine.h"

/* Kernel includes. */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"


/* This variable is not used by this simple Blinky example.  It is defined
purely to allow the project to link as it is used by the full build
configuration. */
volatile unsigned long ulHighFrequencyTickCount = 0UL;

/*-----------------------------------------------------------*/

void vTask1(void *pvParameters)
{
     while(1) {
          PORTC.PODR.BIT.B1 = ~PORTC.PODR.BIT.B1;
          vTaskDelay(100/portTICK_PERIOD_MS);
     }
}

void vTask2(void *pvParameters)
{
     while(1) {
          PORTC.PODR.BIT.B2 = ~PORTC.PODR.BIT.B2;
          vTaskDelay(200/portTICK_PERIOD_MS);
     }
}

void vTask3(void *pvParameters)
{
     while(1) {
          PORTC.PODR.BIT.B3 = ~PORTC.PODR.BIT.B3;
          vTaskDelay(300/portTICK_PERIOD_MS);
     }
}

void main(void)
{
extern void HardwareSetup( void );

     /* Renesas provided CPU configuration routine.  The clocks are configured in
     here. */
     HardwareSetup();

     /* Turn all LEDs off. */
     PORTC.PDR.BYTE = 0xFF;     //ポートC出力
     PORTC.PODR.BYTE = 0xFF;     //初期値


     xTaskCreate(vTask1,"Task1",100,NULL,1,NULL);
     xTaskCreate(vTask2,"Task2",100,NULL,1,NULL);
     xTaskCreate(vTask3,"Task3",100,NULL,1,NULL);


     /* Create the queue. */
     vTaskStartScheduler();

     /* If all is well the next line of code will not be reached as the scheduler
     will be     running.  If the next line is reached then it is likely that there was
     insufficient heap available for the idle task to be created. */
     for( ;; );
}
/*-----------------------------------------------------------*/


/* A callback function named vApplicationSetupTimerInterrupt() must be defined
to configure a tick interrupt source, and configTICK_VECTOR set in
FreeRTOSConfig.h to install the tick interrupt handler in the correct position
in the vector table.  This example uses a compare match timer.  It can be
into any FreeRTOS project, provided the same compare match timer is available. */
void vApplicationSetupTimerInterrupt( void )
{
     /* Enable compare match timer 0. */
     SYSTEM.PRCR.WORD = 0x0A502;
     MSTP( CMT0 ) = 0;
     SYSTEM.PRCR.WORD = 0x0A500;

     /* Interrupt on compare match. */
     CMT0.CMCR.BIT.CMIE = 1;
     CMT0.CMCR.BIT.CKS = 0;                         // CLK = PCLK/8

     /* Set the compare match value. */
     CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ ) -1 ) / 8 );

     /* Divide the PCLK by 8. */
     CMT0.CMCR.BIT.CKS = 0;

     /* Enable the interrupt... */
     _IEN( _CMT0_CMI0 ) = 1;
     ICU.IER[IER_CMT0_CMI0].BIT.IEN_CMT0_CMI0 = 1;     //Enable CMIE
     /* ...and set its priority to the application defined kernel priority. */
     _IPR( _CMT0_CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;
     ICU.IPR[IPR_CMT0_CMI0].BYTE = configKERNEL_INTERRUPT_PRIORITY;          // Set interrupt priority level
     /* Start the timer. */
     CMT.CMSTR0.BIT.STR0 = 1;
}
/*-----------------------------------------------------------*/

/* If configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h, then this
function will be called if pvPortMalloc() returns NULL because it has exhausted
the available FreeRTOS heap space.  See http://www.freertos.org/a00111.html. */
void vApplicationMallocFailedHook( void )
{
     for( ;; );
}
/*-----------------------------------------------------------*/

/* If configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2 in
FreeRTOSConfig.h, then this function will be called if a task overflows its
stack space.  See
http://www.freertos.org/Stacks-and-stack-overflow-checking.html. */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
     for( ;; );
}
/*-----------------------------------------------------------*/

/* If configUSE_IDLE_HOOK is set to 1 in FreeRTOSConfig.h, then this function
will be called on each iteration of the idle task.  See
http://www.freertos.org/a00016.html */
void vApplicationIdleHook( void )
{
     /* If this is being executed then the kernel has been started.  Start the high
     frequency timer test as described at the top of this file.  This is only
     included in the optimised build configuration - otherwise it takes up too much
     CPU time and can disrupt other tests. */
     #ifdef INCLUDE_HIGH_FREQUENCY_TIMER_TEST
     static portBASE_TYPE xTimerTestStarted = pdFALSE;
     extern void vSetupHighFrequencyTimer( void );
          if( xTimerTestStarted == pdFALSE )
          {
               vSetupHighFrequencyTimer();
               xTimerTestStarted = pdTRUE;
          }
     #endif
}
/*-----------------------------------------------------------*/
