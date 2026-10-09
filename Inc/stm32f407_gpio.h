/*
 * stm32f407_gpio.h
 *
 *  Created on: 28-Sept-2026
 *      Author: athwi
 */

#ifndef STM32F407_GPIO_H_
#define STM32F407_GPIO_H_

#include "stm32f407xx.h"

/*
 * Configuration structure of GPIO pin
 */
typedef struct
{
	uint8_t GPIO_PortName;
	uint8_t GPIO_PinNumber;   /*Possible values from @GPIO_PIN_NUMBERS */
	uint8_t GPIO_PinMode;     /*Possible values from @GPIO_PIN_MODES */
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOpType;
	uint8_t GPIO_PinAltFunMode;

}GPIO_PinConfig_t;

/*
 * This is the handle structure for a GPIO pin
 */

typedef struct
{
	GPIO_RegDef_t *pGPIOx;            /* This holds the base address of the GPIO port to which the pin belongs */
	GPIO_PinConfig_t GPIO_PinConfig;  /* This holds the GPIO pin configuration settings */

}GPIO_Handle_t;



/**************************************************************************************************************
  GPIOx Initialization API Macros
 *****************************************************************************************************************/
/*
 * @GPIO PIN NUMBERS
 * Each GPIO port have 16 pins
 */
#define GPIO_PIN_NO_0      0
#define GPIO_PIN_NO_1      1
#define GPIO_PIN_NO_2      2
#define GPIO_PIN_NO_3      3
#define GPIO_PIN_NO_4      4
#define GPIO_PIN_NO_5      5
#define GPIO_PIN_NO_6      6
#define GPIO_PIN_NO_7      7
#define GPIO_PIN_NO_8      8
#define GPIO_PIN_NO_9      9
#define GPIO_PIN_NO_10     10
#define GPIO_PIN_NO_11     11
#define GPIO_PIN_NO_12     12
#define GPIO_PIN_NO_13     13
#define GPIO_PIN_NO_14     14
#define GPIO_PIN_NO_15     15

/*
 * @GPIO PIN MODE
 * This is the 4 modes of GPIO pins, Decided by GPIO_MODER. Each pin take 2 bits. For nth pin = 2n+1 : 2n
 * 3 external interrupt modes are also included in it.
 */

#define GPIO_MODE_IN      0
#define GPIO_MODE_OUT     1
#define GPIO_MODE_ALTFN   2
#define GPIO_MODE_ANALOG  3

#define GPIO_MODE_IT_FT   4  //Falling trigger
#define GPIO_MODE_IT_RT   5  //Rising trigger
#define GPIO_MODE_IT_RFT  6  //Rising and Falling trigger

/*
 * @GPIO OUTPUT TYPES
 * There are 2 output types for a port, decided by GPIOx_OTYPER. Each pin takes 1 bit from 0 to 15
 */
#define GPIO_OP_TYPE_PP   0
#define GPIO_OP_TYPE_OD   1

/*
 * @GPIO OUTPUT SPEEDS
 * 4 Different speed for a port, decided by GPIOx_OSPEEDR. Each pin takes 2 bits. for nth pin = 2n+1 : 2n
 */
#define GPIO_SPEED_LOW         0
#define GPIO_SPEED_MEDIUM      1
#define GPIO_SPEED_HIGH        2
#define GPIO_SPEED_VERY_HIGH   3

/*
 * @GPIO PORT PU PD MODES
 * There are 3 pull up/ pull down modes in a port, decided by GPIOx_PUPDR. Each pin takes 2 bits. for nth pin = 2n+1 : 2n
 */
#define GPIO_NO_PUPD     0
#define GPIO_PU          1
#define GPIO_PD          2



/***********************************************************************************************************************************
 *                                                    APIs supported by GPIO
 ********************************************************************************************************************************/
/*
 * Initialize and De-Initialize the GPIO PIn
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 * For peripheral clock setup of GPIO pin
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi);

/*
 * For read and write operation of GPIO Pins and Ports
 */
uint8_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

/*
 * For Interrupt handling and Interrupt Service Routine
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);






#endif /* STM32F407_GPIO_H_ */
