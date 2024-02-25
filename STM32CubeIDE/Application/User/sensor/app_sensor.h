/*
 * app_sensor.h
 *
 *  Created on: Feb 11, 2024
 *      Author: peppe
 */

#ifndef APPLICATION_USER_SENSOR_APP_SENSOR_H_
#define APPLICATION_USER_SENSOR_APP_SENSOR_H_

#include "stdint.h"

typedef enum
{
  PUMP1_STATUS_ON = 0x01,
  PUMP2_STATUS_ON = 0x02
} PumpStatusMask;

/* LED */
//#define LED_PUMP_1_GROUP GPIOB
#define LED_PUMP_1  GPIO_PIN_8
#define LED_PUMP_2  GPIO_PIN_10

/* flags */
#define THREAD_FLAG_READ_SENSOR    (0x00000001U)
#define THREAD_FLAG_PUMP1_TOGGLE   (0x00000100U)
#define THREAD_FLAG_PUMP2_TOGGLE   (0x00000200U)
#define THREAD_FLAG_PUMPS_TOGGLE   (0x00000400U)


/* version */
#define SENSOR_APP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define SENSOR_APP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define SENSOR_APP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define SENSOR_APP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */


/*Sensor process variable */
#define CFG_APP_SENSOR_PROCESS_NAME                  "SENSOR_PROCESS"
#define CFG_APP_SENSOR_PROCESS_ATTR_BITS             (0)
#define CFG_APP_SENSOR_PROCESS_CB_MEM                (0)
#define CFG_APP_SENSOR_PROCESS_CB_SIZE               (0)
#define CFG_APP_SENSOR_PROCESS_STACK_MEM             (0)
#define CFG_APP_SENSOR_PROCESS_PRIORITY              osPriorityNone
#define CFG_APP_SENSOR_PROCESS_STACK_SIZE            512
#define SENSOR_READ_CYCLE                            12000

/* adc  */
/* Definitions of environment analog values */
/* Value of analog reference voltage (Vref+), connected to analog voltage   */
/* supply Vdda (unit: mV).                                                  */
#define VDDA_APPLI                       ((uint32_t)3300)

/* Definitions of data related to this example */
/* Full-scale digital value with a resolution of 12 bits (voltage range     */
/* determined by analog voltage references Vref+ and Vref-,                 */
/* refer to reference manual).                                              */
#define DIGITAL_SCALE_12BITS             ((uint32_t) 0xFFF)

/* Init variable out of ADC expected conversion data range */
#define VAR_CONVERTED_DATA_INIT_VALUE    (DIGITAL_SCALE_12BITS + 1)

/* Private macro -------------------------------------------------------------*/

/**
  * @brief  Macro to calculate the voltage (unit: mVolt)
  *         corresponding to a ADC conversion data (unit: digital value).
  * @note   ADC measurement data must correspond to a resolution of 12bits
  *         (full scale digital value 4095). If not the case, the data must be
  *         preliminarily rescaled to an equivalent resolution of 12 bits.
  * @note   Analog reference voltage (Vref+) must be known from
  *         user board environment.
  * @param  __VREFANALOG_VOLTAGE__ Analog reference voltage (unit: mV)
  * @param  __ADC_DATA__ ADC conversion data (resolution 12 bits)
  *                       (unit: digital value).
  * @retval ADC conversion data equivalent voltage value (unit: mVolt)
  */
#define __ADC_CALC_DATA_VOLTAGE(__VREFANALOG_VOLTAGE__, __ADC_DATA__)       \
  ((__ADC_DATA__) * (__VREFANALOG_VOLTAGE__) / DIGITAL_SCALE_12BITS)


// ---- function ----- //
void Sensor_Init(void);
//static uint32_t ADC_ReadChannels(uint32_t channel);
uint16_t SYS_GetVoltLevel(void);


#endif /* APPLICATION_USER_SENSOR_APP_SENSOR_H_ */
