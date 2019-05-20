#include <flex_sensor.h>

void flexInit(ADC_TypeDef* target) {
  ADC_InitTypeDef initStruct = {
    .ADC_Mode = ADC_Mode_Independent,
    .ADC_ScanConvMode = ENABLE,
    .ADC_ContinuousConvMode = ENABLE,
    .ADC_ExternalTrigConv = ADC_ExternalTrigConvCmd,
    .ADC_DataAlign = ADC_DataAlign_Right,
    .ADC_NbrOfChannel = 2,
  };
  ADC_DeInit(target);
}