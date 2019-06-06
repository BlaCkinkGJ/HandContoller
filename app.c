#include <includes.h>

#define JSON_FORMAT "{\"flex\":{\"1\":%d,\"2\":%d,\"3\":%d},\"Gyro\":{\"X\":%d,\"Y\":%d,\"Z\":%d},\"Accel\":{\"X\":%d,\"Y\":%d,\"Z\":%d}}"
// local variables

static OS_TCB AppTaskStartTCB;
static OS_TCB DebugMonitorTCB;
static OS_TCB GyroSensorTCB;

// stacks

static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static CPU_STK DebugMonitorStk[APP_TASK_STK_SIZE];
static CPU_STK GyroSensorStk[APP_TASK_STK_SIZE];

// function prototype

static void AppTaskCreate(void);
static void AppTaskStart(void* p_arg);

static void DebugMonitor(void* p_arg);
static void GyroSensor(void* p_arg);

static void USART1_IRQHandler()
{
    unsigned char recvBuf = '\0';

    // you have to change this section
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    recvBuf = (unsigned char)USART_ReceiveData(USART1);
    USART_SendData(USART1, recvBuf);
    USART_SendData(USART2, recvBuf);

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

static void USART2_IRQHandler()
{
    unsigned char recvBuf = '\0';

    // you have to change this section
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        ;
    recvBuf = (unsigned char)USART_ReceiveData(USART2);
    USART_SendData(USART1, recvBuf);

    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

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
    OSTaskCreate((OS_TCB*)&DebugMonitorTCB,
        (CPU_CHAR*)"App Task First",
        (OS_TASK_PTR)DebugMonitor,
        (void*)0,
        (OS_PRIO)3,
        (CPU_STK*)&DebugMonitorStk[0],
        (CPU_STK_SIZE)APP_TASK_STK_SIZE / 10,
        (CPU_STK_SIZE)APP_TASK_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)5, /* Time Qunanta(This for Round Robin) */
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err);
    OSTaskCreate((OS_TCB*)&GyroSensorTCB,
        (CPU_CHAR*)"App Task First",
        (OS_TASK_PTR)GyroSensor,
        (void*)0,
        (OS_PRIO)3,
        (CPU_STK*)&GyroSensorStk[0],
        (CPU_STK_SIZE)APP_TASK_STK_SIZE / 10,
        (CPU_STK_SIZE)APP_TASK_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)5, /* Time Qunanta(This for Round Robin) */
        (void*)0,
        (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
        (OS_ERR*)&err);
}

static CPU_INT16S AccelGyro[6] = { 0 };
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
    // accel_x,y,z; gyro_x,y,z

    void* pack[4] = { NULL, NULL, NULL, NULL };
    struct DebugContents contents;
    while (DEF_TRUE) {
        static char flexString[256];
        static char gyroString[256];
        static char accelString[256];
        if (MPU6050_TestConnection() != 0) {
            MPU6050_GetRawAccelGyro(AccelGyro);
        }
        sprintf(flexString, "%-4d%-4d%-4d", getFlexValue(0), getFlexValue(1), getFlexValue(2));
        sprintf(gyroString, "%-6d%-6d%-6d", AccelGyro[0], AccelGyro[1], AccelGyro[2]);
        sprintf(accelString, "%-6d%-6d%-6d", AccelGyro[3], AccelGyro[4], AccelGyro[5]);
        pack[0] = flexString;
        pack[2] = gyroString;
        pack[3] = accelString;
        setContents(&contents, pack);
        drawTitle("flex Mode");
        drawHeader();
        drawContents(&contents);
        //@TODO: Change the message queue to refresh the screen
        OSTimeDlyHMSM(0, 0, 0, 500,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

static void GyroSensor(void* p_arg)
{
    OS_ERR err;

    while (DEF_TRUE) {
        static char buffer[1024];
        if (MPU6050_TestConnection() != 0) {
            MPU6050_GetRawAccelGyro(AccelGyro);
        }
        sprintf(buffer, JSON_FORMAT, getFlexValue(0), getFlexValue(1), getFlexValue(2),
            AccelGyro[0], AccelGyro[1], AccelGyro[2],
            AccelGyro[3], AccelGyro[4], AccelGyro[5]);
        UART_SendStr(USART2, (const char*)buffer);
        OSTimeDlyHMSM(0, 0, 0, 50,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}