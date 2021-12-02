/**
 * @file adc.h
 * @author yokeshwaran gunasekaran (yokeshwaran.gunasekaran@ltts.com)
 * @brief Functions to handle ADC peripheral
 * @version 0.1
 * @date 2021-12-1
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef ADC_H_
#define ADC_H_

#include "project_config.h"

/**
 * @brief Function to initialise the analog to digital converter (ADC)
 * 
 */
void adc_init();

/**
 * @brief Funciton to read analog pin and output it to a register ADC which has two 8 bit registers ADCL and ADCH.
 * 
 * @param channel ADC channel on which conversion must be done and value must be read
 * @return uint16_t ADC value
 */
uint16_t adc_read(uint8_t channel);

#endif
