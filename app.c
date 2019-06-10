#include <includes.h>

// local variables

static OS_TCB AppTaskStartTCB;
static OS_TCB DebugMonitorTCB;
static OS_TCB FlexSensorTCB;
static OS_TCB GyroSensorTCB;
static OS_TCB SendBluetoothTCB;

// stacks
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static CPU_STK DebugMonitorStk[APP_TASK_STK_SIZE];
static CPU_STK FlexSensorStk[APP_TASK_STK_SIZE];
static CPU_STK GyroSensorStk[APP_TASK_STK_SIZE];
static CPU_STK SendBluetoothStk[APP_TASK_STK_SIZE];

// function prototype

static void AppTaskCreate(void);
static void AppTaskStart(void* p_arg);

static void DebugMonitor(void* p_arg);
static void FlexSensor(void* p_arg);
static void GyroSensor(void* p_arg);
static void SendBluetooth(void* p_arg);

// global variables
static struct DebugContents contents;

OS_Q Flex_Q;
OS_Q Gyro_Q;

OS_MEM MemoryPartition;
CPU_INT16S MemoryPartitionStorage[PARTITION_QTY][PARTITION_SIZE];

/**
 * @brief main function.
 * 
 * @return int 
 */
int main(void)
{
    OS_ERR err;

    BSP_IntDisAll(); /* Disable all interrupts. */

    OSInit(&err); /* Init uC/OS-III. */

#ifdef ROUND_ROBIN
    OSSchedRoundRobinCfg((CPU_BOOLEAN)DEF_TRUE,
        (OS_TICK)10,
        (OS_ERR*)&err);
#endif

    OSTaskCreate((OS_TCB*)&AppTaskStartTCB, /* Create the start task */
        (CPU_CHAR*)"App Task Start",
        (OS_TASK_PTR)AppTaskStart,
        (void*)0,
        (OS_PRIO)APP_TASK_START_PRIO,
        (CPU_STK*)&AppTaskStartStk[0],
        (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
        (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)0,
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err);

    OSStart(&err); /* Start multitasking (i.e. give control to uC/OS-III). */
}

/**
 * @brief Startup task. This task is necessary to start the program
 * 
 * @param p_arg 
 */
static void AppTaskStart(void* p_arg)
{
    CPU_INT32U cpu_clk_freq;
    CPU_INT32U cnts;
    OS_ERR err;

    (void)p_arg;

    BSP_Init(); /* Initialize BSP functions */
    CPU_Init();

    flexInit();
    MPU6050_I2C_Init();
    MPU6050_Initialize();
    LCD_Init(); /* Initialize TFT-LCD */
    // Touch_Configuration(); /* Initialize TFT-LCD configuration */
    // Touch_Adjust();
    LCD_Clear(WHITE); /* Refresh TFT-LCD screen to WHITE */
    UART_CNF(); /* Initialize USART */
    UART_NVIC_Init();
    cpu_clk_freq = BSP_CPU_ClkFreq();
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz; /* Determine nbr SysTick increments */
    OS_CPU_SysTickInit(cnts); /* Init uC/OS periodic time src (SysTick). */

    BSP_IntVectSet(BSP_INT_ID_USART1, USART1_IRQHandler);
    BSP_IntEn(BSP_INT_ID_USART1);
    BSP_IntVectSet(BSP_INT_ID_USART2, USART2_IRQHandler);
    BSP_IntEn(BSP_INT_ID_USART2);

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err); /* Compute CPU capacity with no task running */
#endif
    AppTaskCreate(); /* Create application tasks */

    BSP_LED_Off(0);
    while (DEF_TRUE) { /* Task body, always written as an infinite loop. */
        OSTimeDlyHMSM(0, 0, 0, 250,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

/**
 * @brief Create the app task. In this section, we can make the user defined task.
 * 
 */
static void AppTaskCreate(void)
{
    OS_ERR err;
    UART_SendStr(USART2, "AT+BTSCAN");

    OSQCreate((OS_Q *)&Flex_Q,
        (CPU_CHAR *)"Flex queue",
        (OS_MSG_QTY)FLEX_MSG_Q,
        (OS_ERR *)&err);
    OSQCreate((OS_Q *)&Gyro_Q,
        (CPU_CHAR *)"Gyro queue",
        (OS_MSG_QTY)GYRO_MSG_Q,
        (OS_ERR *)&err);

    OSMemCreate((OS_MEM *)&MemoryPartition,
        (CPU_CHAR *)"Memory Partition",
        (void *)&MemoryPartitionStorage[0][0],
        (OS_MEM_QTY) PARTITION_QTY,
        (OS_MEM_SIZE) PARTITION_SIZE,
        (OS_ERR *)&err);
    
    OSTaskCreate((OS_TCB*)&DebugMonitorTCB,
        (CPU_CHAR*)"DebugMonitor",
        (OS_TASK_PTR)DebugMonitor,
        (void*)0,
        (OS_PRIO)6,
        (CPU_STK*)&DebugMonitorStk[0],
        (CPU_STK_SIZE)APP_TASK_STK_SIZE / 10,
        (CPU_STK_SIZE)APP_TASK_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)5, /* Time Qunanta(This for Round Robin) */
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err);
    OSTaskCreate((OS_TCB*)&GyroSensorTCB,
        (CPU_CHAR*)"Gyro Sensor Task",
        (OS_TASK_PTR)GyroSensor,
        (void*)0,
        (OS_PRIO)4,
        (CPU_STK*)&GyroSensorStk[0],
        (CPU_STK_SIZE)APP_TASK_STK_SIZE / 10,
        (CPU_STK_SIZE)APP_TASK_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)5, /* Time Qunanta(This for Round Robin) */
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err);
    OSTaskCreate((OS_TCB*)&FlexSensorTCB,
        (CPU_CHAR*)"Flex Sensor Task",
        (OS_TASK_PTR)FlexSensor,
        (void*)0,
        (OS_PRIO)4,
        (CPU_STK*)&FlexSensorStk[0],
        (CPU_STK_SIZE)APP_TASK_STK_SIZE / 10,
        (CPU_STK_SIZE)APP_TASK_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)5, /* Time Qunanta(This for Round Robin) */
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err);
    OSTaskCreate((OS_TCB*)&SendBluetoothTCB,
        (CPU_CHAR*)"Message sender",
        (OS_TASK_PTR)SendBluetooth,
        (void*)0,
        (OS_PRIO)3,
        (CPU_STK*)&SendBluetoothStk[0],
        (CPU_STK_SIZE)APP_TASK_STK_SIZE / 10,
        (CPU_STK_SIZE)APP_TASK_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)5, /* Time Qunanta(This for Round Robin) */
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err);
}

/**
 * @brief Test the sensors template. You use this task to test the program.
 * If you test all the sensors then you have to erase this code and change the
 * other code.
 * 
 * @param p_arg 
 */
static void DebugMonitor(void* p_arg)
{
    OS_ERR err;
    while (DEF_TRUE) {
        drawTitle("Debug Monitor");
        drawHeader();
        drawContents(&contents);
        OSTimeDlyHMSM(0, 0, 0, 500,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

static void SendBluetooth(void* p_arg)
{
    OS_ERR err;
    OS_PEND_DATA myPendTable[PEND_TABLE_SIZE];
    
    char buffer[1024];
    CPU_INT16S *flexDataBlkPtr = NULL;
    CPU_INT16S *gyroDataBlkPtr = NULL;

    while (DEF_TRUE) {
        myPendTable[0].PendObjPtr = (OS_PEND_OBJ *)&Flex_Q;
        myPendTable[1].PendObjPtr = (OS_PEND_OBJ *)&Gyro_Q;
        OSPendMulti((OS_PEND_DATA *)&myPendTable[0],
            (OS_OBJ_QTY) PEND_TABLE_SIZE,
            (OS_TICK) PEND_TABLE_TIMEOUT,
            (OS_OPT) OS_OPT_PEND_BLOCKING,
            (OS_ERR *) &err);
        
        if (myPendTable[0].RdyObjPtr != NULL) {
            if (flexDataBlkPtr != NULL) {
                OSMemPut((OS_MEM *)&MemoryPartition, 
                    (void *)flexDataBlkPtr,
                    (OS_ERR *)&err);
            }
            flexDataBlkPtr = myPendTable[0].RdyMsgPtr;
        }

        if (myPendTable[1].RdyObjPtr != NULL) {
            if (gyroDataBlkPtr != NULL) {
                OSMemPut((OS_MEM *)&MemoryPartition, 
                    (void *)gyroDataBlkPtr,
                    (OS_ERR *)&err);
            }
            gyroDataBlkPtr = myPendTable[1].RdyMsgPtr;
        }
        if (flexDataBlkPtr != NULL && gyroDataBlkPtr != NULL) {
          sprintf(buffer, JSON_FORMAT
              , flexDataBlkPtr[0], flexDataBlkPtr[1], flexDataBlkPtr[2]
              , gyroDataBlkPtr[0], gyroDataBlkPtr[1], gyroDataBlkPtr[2]
              , gyroDataBlkPtr[3], gyroDataBlkPtr[4], gyroDataBlkPtr[5]);
          setContents(&contents, flexDataBlkPtr, gyroDataBlkPtr);
          UART_SendStr(USART2, (const char*)buffer);
        }
    }
}

static void FlexSensor(void* p_arg)
{
    OS_ERR err;
    CPU_INT16S *flexDataBlkPtr;

    while (DEF_TRUE) {
        // must be put
        flexDataBlkPtr = (CPU_INT16S *)OSMemGet((OS_MEM *)&MemoryPartition, (OS_ERR *)&err);
        if (err == OS_ERR_NONE) {
            flexDataBlkPtr[0] = getFlexValue(0);
            flexDataBlkPtr[1] = getFlexValue(1);
            flexDataBlkPtr[2] = getFlexValue(2);
            OSQPost((OS_Q *)&Flex_Q,
                (void *)flexDataBlkPtr,
                (OS_MSG_QTY)sizeof(void *),
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&err);
        }
        OSTimeDlyHMSM(0, 0, 0, 5,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

static void GyroSensor(void* p_arg)
{
    OS_ERR err;
    CPU_INT16S *gyroDataBlkPtr;

    while (DEF_TRUE) {
        gyroDataBlkPtr = (CPU_INT16S *)OSMemGet((OS_MEM *)&MemoryPartition, (OS_ERR *)&err);
        if (err == OS_ERR_NONE) {
            if (MPU6050_TestConnection() != 0) {
                MPU6050_GetRawAccelGyro(gyroDataBlkPtr);
            }
            OSQPost((OS_Q *)&Gyro_Q,
                (void *)gyroDataBlkPtr,
                (OS_MSG_QTY)sizeof(void *),
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&err);
        }
        OSTimeDlyHMSM(0, 0, 0, 5,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}