#include <includes.h>

#define THRESHOLD 1000000

// local variables

static  OS_TCB          AppTaskStartTCB;
static  OS_TCB          TestTaskTCB; 

// stacks

static  CPU_STK         AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK         TestTaskStk[APP_TASK_STK_SIZE];

// function prototype

static  void  AppTaskCreate     (void);
static  void  AppTaskStart      (void *p_arg);

static  void  TestTask (void *p_arg);

/**
 * @brief main function.
 * 
 * @return int 
 */
int  main (void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSSchedRoundRobinCfg((CPU_BOOLEAN)DEF_TRUE, 
                         (OS_TICK    )10,
                         (OS_ERR    *)&err);
    
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR )AppTaskStart, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}

/**
 * @brief Startup task. This task is necessary to start the program
 * 
 * @param p_arg 
 */
static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
    
    

   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                                 */

    CPU_Init(); 
    UART_CNF();                                                 /* Initialize USART                                         */

    cpu_clk_freq = BSP_CPU_ClkFreq();
    cnts         = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;/* Determine nbr SysTick increments                         */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running                */
#endif

    CPU_IntDisMeasMaxCurReset();
    
    AppTaskCreate();                                            /* Create application tasks                                 */

    BSP_LED_Off(0);
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
        OSTimeDlyHMSM(0, 0, 0, 250, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
}

/**
 * @brief Create the app task. In this section, we can make the user defined task.
 * 
 */
static  void  AppTaskCreate (void)
{
    OS_ERR  err;
    

    
    OSTaskCreate((OS_TCB     *)&TestTaskTCB, 
                 (CPU_CHAR   *)"App Task First",
                 (OS_TASK_PTR )TestTask, 
                 (void       *)0,
                 (OS_PRIO     )3,
                 (CPU_STK    *)&TestTaskStk[0],
                 (CPU_STK_SIZE)APP_TASK_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )2,  /* Time Qunanta */
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}

/**
 * @brief Test the sensors template. You use this task to test the program.
 * If you test all the sensors then you have to erase this code and change the
 * other code.
 * 
 * @param p_arg 
 */
void TestTask(void *p_arg) {
    static int counter = 0;
    while (DEF_TRUE) {
      if(++counter == THRESHOLD){ /* check the threshold */
        BSP_LED_Toggle(1);
        counter = 0;
      }
      UART_SendData(USART1, '*');
        
    }
}
