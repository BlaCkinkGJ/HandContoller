#include <flex_sensor.h>

/**
 * @brief Referenced by DMA module.
 * If you handle the this value.
 * then you watch out the concurrent situation.
 */
static vu32 flexValue[3];

/**
 * @brief RCC initialization
 * 
 */
static void
flex_RCC_init() {
    RCC_APB2PeriphClockCmd((RCC_APB2Periph_AFIO
                         | RCC_APB2Periph_GPIOC
                         | RCC_APB2Periph_GPIOD
                         | RCC_APB2Periph_ADC1),ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/**
 * @brief GPIO initialization about the flex sensor
 * Currently, ONLY THREE sensor can use
 * 
 * @param GPIO 
 * @param GPIO_Pin 
 */
static void
flex_GPIO_Init(GPIO_TypeDef* GPIO, u16 GPIO_Pin[3]) {
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin   = GPIO_Pin[0] | GPIO_Pin[1] | GPIO_Pin[2],
        .GPIO_Speed = GPIO_Speed_10MHz,
        .GPIO_Mode  = GPIO_Mode_AIN
    };

    GPIO_Init(GPIO, &GPIO_InitStructure);
}

/**
 * @brief ADC initialization about the flex sensor
 * Currently, ONLY THREE sensor can use * * @param ADC * @param ADC_Channel */
static void
flex_ADC_Init(ADC_TypeDef* ADC, u8 ADC_Channel[3]){
    ADC_InitTypeDef ADC_InitStructure = {
        .ADC_Mode               = ADC_Mode_Independent,
        .ADC_ScanConvMode       = ENABLE,
        .ADC_ContinuousConvMode = ENABLE,
        .ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None,
        .ADC_DataAlign          = ADC_DataAlign_Right,
        .ADC_NbrOfChannel       = 3
    };

    ADC_DeInit(ADC);
    ADC_RegularChannelConfig(ADC, ADC_Channel[0], 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC, ADC_Channel[1], 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC, ADC_Channel[2], 3, ADC_SampleTime_239Cycles5);
    ADC_Init(ADC, &ADC_InitStructure);
    ADC_DMACmd(ADC, ENABLE);
    ADC_Cmd(ADC, ENABLE);
}

/**
 * @brief DMA initialization about the flex sensor
 * Currently, ONLY THREE sensor can use
 * 
 * @param ADC 
 * @param DMA_Channel 
 */
static void
flex_DMA_Init(ADC_TypeDef* ADC, DMA_Channel_TypeDef *DMA_Channel) {
    DMA_InitTypeDef DMA_InitStructure = {
        .DMA_PeripheralBaseAddr = (u32)&ADC->DR,
        .DMA_MemoryBaseAddr     = (vu32) flexValue,
        .DMA_DIR                = DMA_DIR_PeripheralSRC,
        .DMA_BufferSize         = 3,
        .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
        .DMA_MemoryInc          = DMA_MemoryInc_Enable,
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word,
        .DMA_MemoryDataSize     = DMA_MemoryDataSize_Word,
        .DMA_Mode               = DMA_Mode_Circular,
        .DMA_Priority           = DMA_Priority_High,
        .DMA_M2M                = DMA_M2M_Disable
    };
    DMA_DeInit(DMA_Channel);
    DMA_Init(DMA_Channel, &DMA_InitStructure);
    DMA_Cmd(DMA_Channel, ENABLE);
}

/**
 * @brief Start the ADC
 * 
 * @param ADC 
 */
static void
flex_ADC_Start(ADC_TypeDef* ADC) {
    ADC_ResetCalibration(ADC);
    while(ADC_GetResetCalibrationStatus(ADC));
    ADC_StartCalibration(ADC);
    while(ADC_GetCalibrationStatus(ADC));
    ADC_SoftwareStartConvCmd(ADC, ENABLE);
}

/**
 * @brief flex initialization
 * Currently, this set the PC1(port C1) 
 * 
 */
void flexInit() {
    
    u8 ADC_Channels[3] = {
        ADC_Channel_11,
        ADC_Channel_12,
        ADC_Channel_13
    };
    u16 GPIO_Pins[3] = {
        GPIO_Pin_1,
        GPIO_Pin_2,
        GPIO_Pin_3
    };
    flex_RCC_init();
    flex_GPIO_Init(GPIOC, GPIO_Pins);
    flex_ADC_Init(ADC1, ADC_Channels);
    flex_DMA_Init(ADC1, DMA1_Channel1);
    flex_ADC_Start(ADC1);
}

/**
 * @brief Get the Flex Value
 * 
 * @return int 
 */
int getFlexValue(int index) {
    if (index < 0 || index >= 3)
        return -1;
    return (int)(flexValue[index]);
}