/*
 * app_sensor.c
 *
 *  Created on: Feb 11, 2024
 *      Author: peppe
 */

#include "app_sensor.h"
#include "app_version.h"
#include "cmsis_os2.h"
#include "utilities_conf.h"
#include "sys_app.h"
#include "main.h"
#include "stm32_timer.h"
#include "adc.h"

/**
 * adc
 */
extern ADC_HandleTypeDef hadc;
__IO   uint16_t   uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE; /* ADC group regular conversion data */

/* Variables for ADC conversion data computation to physical values */
uint16_t   uhADCxConvertedData_Voltage_mVolt = 0;  /* Value of voltage calculated from ADC conversion data (unit: mV) */

/* Variable to report status of ADC group regular unitary conversion          */
/*  0: ADC group regular unitary conversion is not completed                  */
/*  1: ADC group regular unitary conversion is completed                      */
/*  2: ADC group regular unitary conversion has not been started yet          */
/*     (initial state)                                                        */
__IO   uint8_t ubAdcGrpRegularUnitaryConvStatus = 2; /* Variable set into ADC interruption callback */

/**
 * Process
 */
osThreadId_t Thd_SensorProcessId;
const osThreadAttr_t Thd_SensorProcess_attr =
{
  .name = CFG_APP_SENSOR_PROCESS_NAME,
  .attr_bits = CFG_APP_SENSOR_PROCESS_ATTR_BITS,
  .cb_mem = CFG_APP_SENSOR_PROCESS_CB_MEM,
  .cb_size = CFG_APP_SENSOR_PROCESS_CB_SIZE,
  .stack_mem = CFG_APP_SENSOR_PROCESS_STACK_MEM,
  .priority = CFG_APP_SENSOR_PROCESS_PRIORITY,
  .stack_size = CFG_APP_SENSOR_PROCESS_STACK_SIZE
};

static void Thd_SensorProcess(void *argument);
void SensorProcess( void );
static void OnSensorTimerEvent(void *context);

/**
  * @brief Timer to handle the application Tx
  */
static UTIL_TIMER_Object_t SensorTimer;
static UTIL_TIMER_Time_t SensorPeriodicity = SENSOR_READ_CYCLE;

void Sensor_Init(void)
{

  APP_LOG(TS_OFF, VLEVEL_M, "SENSOR_APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(SENSOR_APP_VERSION_MAIN),
          (uint8_t)(SENSOR_APP_VERSION_SUB1),
          (uint8_t)(SENSOR_APP_VERSION_SUB2));

//  if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)
//  {
//    /* Calibration Error */
//    Error_Handler();
//  }

  /* initialize process */
  Thd_SensorProcessId = osThreadNew(Thd_SensorProcess, NULL, &Thd_SensorProcess_attr);
  if (Thd_SensorProcessId == NULL)
  {
    Error_Handler();
  }

  /* timer to wakeup process */
  UTIL_TIMER_Create(&SensorTimer, SensorPeriodicity, UTIL_TIMER_ONESHOT, OnSensorTimerEvent, NULL);
  UTIL_TIMER_Start(&SensorTimer);

}


static void Thd_SensorProcess(void *argument)
{

  UNUSED(argument);
  for (;;)
  {
    osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
    SensorProcess(); /*what you want to do*/
  }
}

void SensorProcess( void )
{

  /* Reset status variable of ADC group regular unitary conversion before   */
  /* performing a new ADC group regular conversion start.                   */
  ubAdcGrpRegularUnitaryConvStatus = 0;

  //uint32_t ret = ADC_ReadChannels(ADC_CHANNEL_7);

  uhADCxConvertedData_Voltage_mVolt = SYS_GetVoltLevel();
  APP_LOG(TS_OFF, VLEVEL_M, "SENSOR_LEVEL: mV %d \r\n",
      (uint16_t)(uhADCxConvertedData_Voltage_mVolt));

//  /* Start ADC group regular conversion with IT */
//  if (HAL_ADC_Start_IT(&hadc) != HAL_OK)
//  {
//    /* ADC conversion start error */
//    Error_Handler();
//  }
//
//  /* Wait till conversion is done */
//  while (ubAdcGrpRegularUnitaryConvStatus == 0);

}


static uint32_t ADC_ReadChannels(uint32_t channel)
{
  /* USER CODE BEGIN ADC_ReadChannels_1 */

  /* USER CODE END ADC_ReadChannels_1 */
  uint32_t ADCxConvertedValues = 0;
  ADC_ChannelConfTypeDef sConfig = {0};

  MX_ADC_Init();

  /* Start Calibration */
  if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure Regular Channel */
  sConfig.Channel = channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_ADC_Start(&hadc) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }
  /** Wait for end of conversion */
  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);

  /** Wait for end of conversion */
  HAL_ADC_Stop(&hadc);   /* it calls also ADC_Disable() */

  ADCxConvertedValues = HAL_ADC_GetValue(&hadc);

  HAL_ADC_DeInit(&hadc);

  return ADCxConvertedValues;
  /* USER CODE BEGIN ADC_ReadChannels_2 */

  /* USER CODE END ADC_ReadChannels_2 */
}

uint16_t SYS_GetVoltLevel(void)
{
  /* USER CODE BEGIN SYS_GetBatteryLevel_1 */

  /* USER CODE END SYS_GetBatteryLevel_1 */
  uint16_t levelmV = 0;
  uint32_t measuredLevel = 0;

  measuredLevel = ADC_ReadChannels(ADC_CHANNEL_7);

  if (measuredLevel == 0)
  {
    levelmV = 0;
  }
  else
  {
    levelmV = __ADC_CALC_DATA_VOLTAGE(VDDA_APPLI, measuredLevel);
  }

  return levelmV;
  /* USER CODE BEGIN SYS_GetBatteryLevel_2 */

  /* USER CODE END SYS_GetBatteryLevel_2 */
}


static void OnSensorTimerEvent(void *context)
{
  /* USER CODE BEGIN OnTxTimerEvent_1 */

  /* USER CODE END OnTxTimerEvent_1 */
  osThreadFlagsSet(Thd_SensorProcessId, 1);

  /*Wait for next tx slot*/
  UTIL_TIMER_Start(&SensorTimer);
  /* USER CODE BEGIN OnTxTimerEvent_2 */

  /* USER CODE END OnTxTimerEvent_2 */
}


/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  hadc: ADC handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  /* Retrieve ADC conversion data */
  uhADCxConvertedData = HAL_ADC_GetValue(hadc);

  /* Computation of ADC conversions raw data to physical values           */
  /* using helper macro.                                                  */
  uhADCxConvertedData_Voltage_mVolt = __ADC_CALC_DATA_VOLTAGE(VDDA_APPLI, uhADCxConvertedData);

  /* Update status variable of ADC unitary conversion                     */
  ubAdcGrpRegularUnitaryConvStatus = 1;

}

