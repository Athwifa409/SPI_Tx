/*
 * stm32f407_gpio.c
 *
 *  Created on: 28-Sept-2026
 *      Author: athwi
 */


#include "stm32f407_gpio.h"
#include "stm32f407xx.h"
#include "stm32f407_spi.h"


/*
 * The API of GPIO driver.h is
 */
/***************************************************************************************************************************
 * @Function - GPIO_Init
 * @Brief    - This function initializes the GPIO peripheral in use
 * @Param1   - GPIO Handler function/address
 * @Param2   -
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0;

	//Enable peripheral clock
	GPIO_PeriClockControl(pGPIOHandle ->pGPIOx, ENABLE);

	//1.Configure the mode of GPIO pin
	if(pGPIOHandle ->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)                                                   // PinMode <= 3
	{                                                                                                                   // temp = PinMode << (2 * PinNumber) ; pinMode value is given to MODER position, takes 2 bits
		//non interrupt modes                                                                                           // MODER &= ~(0x3 << (2 * PinNumber)) ; Cleared MODER previous bits
		temp = (pGPIOHandle ->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber));        // MODER |= temp ; Giving temp value as new MODER
		pGPIOHandle -> pGPIOx ->MODER &= ~(0x3 << (2 * pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle ->pGPIOx ->MODER |= temp;
		temp = 0;

	}else
	{
		if(pGPIOHandle ->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)                           // These are interrupt modes > 3
		{                                                                                          // for FT mode, Configure FTSR and Clear RTSR
			//1. Configure FTSR bit                                                                // Each pin takes 1 bit, which is corresponding to its pin number
			EXTI ->FTSR |= (1 << pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);                     // FTSR/RTSR |= (1 << PinNumber) ; Setting the bit
			//Clear RTSR bit                                                                       // FTSR/RTSR &= ~(1 << PinNumber) ; Clearing the bit
			EXTI ->RTSR &= ~(1 << pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);

		}else if(pGPIOHandle ->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			//1. Configure RTSR bit
			EXTI ->RTSR |= (1 << pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);
			//Clear FTSR bit
			EXTI ->FTSR &= ~(1 << pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);

		}else if(pGPIOHandle ->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			//Configure both FTSR and RTSR
			EXTI ->FTSR |= (1 << pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);
			EXTI ->RTSR |= (1 << pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);

		}

		//2.Configure the GPIO port selector in SYSCFG_EXTICR                           // This defines which pin is getting the interrupt
		uint8_t temp1 = pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber / 4;                // EXTICR decide which pin is getting the interrupt.
		uint8_t temp2 = pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber % 4;                // It is divided into 4 groups(total 16 bits) of 4 bits, these are EXTICR[], Each EXTICR[] select 4 pins.
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle ->pGPIOx);                 // temp1 = PinNumber / 4 ; which gives the EXTICR[] we need to use
		SYSCFG_PCLK_EN();
		SYSCFG ->EXTICR[temp1] &= ~(0xF << (temp2 * 4));                                // temp2 = PinNumber % 4 ; which gives the bit in EXTICR[]
		SYSCFG ->EXTICR[temp1] |= portcode << (temp2 * 4);                               // PORTCODE GIVES THE GPIO port we are using. EXTICR[temp1] = portcode << (temp2 * 4) ; select port and pin where interrupt is occuring

		//3. Configure the EXTI interrupt delivery using IMR, IMR controls whether exti line is allowed to deliver an interrupt
		EXTI ->IMR |= (1 << pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);

	}

	temp = 0;

	//2. Configure the speed settings
	temp = (pGPIOHandle ->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle ->pGPIOx ->OSPEEDR &= ~(pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle ->pGPIOx ->OSPEEDR |= temp;

	temp = 0;

	//3. Configure the PUPD settings
	temp = (pGPIOHandle ->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle ->pGPIOx ->PUPDR &= ~(pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle ->pGPIOx ->PUPDR |= temp;

	temp = 0;

	//4. Configure the output type
	temp = (pGPIOHandle ->GPIO_PinConfig.GPIO_PinOpType << (2 * pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle ->pGPIOx ->OTYPER &= ~(pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle ->pGPIOx ->OTYPER |= temp;

	temp = 0 ;

	//5. Configure the alternate functionality settings
	if(pGPIOHandle ->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)              // There are 2 AFR registers in a GPIO : AFRL & AFRH
	{                                                                             // Pin 0 - 7 gets alternate fn mode through AFRL, and pin 8 - 15 takes value from AFRH
		//Configure the alternate functionality register                          // Each pin takes 4 bits for their Alternate fn mode. so 1 AFR is divided into 8 group
		                                                                          // temp1 = PinNumber / 8 ; select the AFR register
		uint8_t temp1, temp2;                                                     // temp2 = PinNumber % 8 ; Select the group/bits we need to change
		temp1 = pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber / 8;                  // 4 * temp2 bcz, each pin takes 4 bits in a group
		temp2 = pGPIOHandle ->GPIO_PinConfig.GPIO_PinNumber % 8;                  // AFR[temp1] &= ~(0xf << (4 * temp2)) ; clearing the 4 bits of the AFR which is related to
		pGPIOHandle ->pGPIOx ->AFR[temp1] &= ~(0xF << (4 * temp2));               // AFR[temp1] |= (AltFunMode << (4 * temp2))
		pGPIOHandle ->pGPIOx ->AFR[temp1] |= (pGPIOHandle ->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));

	}

}

/***************************************************************************************************************************
 * @Function - GPIO_DeInit
 * @Brief    - This function deinitializes the GPIO pin. Deinitialization means returns it to its reset state.
 * @Param1   - GPIO base address
 * @Param2   -
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA){
		GPIOA_REG_RESET();
	}else if(pGPIOx == GPIOB){
		GPIOB_REG_RESET();
	}else if(pGPIOx == GPIOC){
		GPIOC_REG_RESET();
	}else if(pGPIOx == GPIOD){
		GPIOD_REG_RESET();
	}else if(pGPIOx == GPIOE){
		GPIOE_REG_RESET();
	}else if(pGPIOx == GPIOF){
		GPIOF_REG_RESET();
	}else if(pGPIOx == GPIOG){
		GPIOG_REG_RESET();
	}else if(pGPIOx == GPIOH){
		GPIOH_REG_RESET();
	}else if(pGPIOx == GPIOI){
		GPIOI_REG_RESET();
	}

}

/***************************************************************************************************************************
 * @Function - GPIO_PeriClockControl
 * @Brief    - They control the peripheral clock of the GPIO pins
 * @Param1   - GPIO base address
 * @Param2   - ENABLE or DISABLE macros
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE){
		if(pGPIOx == GPIOA){
			GPIOA_PCLK_EN();
		}else if(pGPIOx == GPIOB){
			GPIOB_PCLK_EN();
		}else if(pGPIOx == GPIOC){
			GPIOC_PCLK_EN();
		}else if(pGPIOx == GPIOD){
			GPIOD_PCLK_EN();
		}else if(pGPIOx == GPIOE){
			GPIOE_PCLK_EN();
		}else if(pGPIOx == GPIOF){
			GPIOF_PCLK_EN();
		}else if(pGPIOx == GPIOG){
			GPIOG_PCLK_EN();
		}else if(pGPIOx == GPIOH){
			GPIOH_PCLK_EN();
		}else if(pGPIOx == GPIOI){
			GPIOI_PCLK_EN();
		}

	}else{
		if(pGPIOx == GPIOA){
			GPIOA_PCLK_DI();
		}else if(pGPIOx == GPIOB){
			GPIOB_PCLK_DI();
		}else if(pGPIOx == GPIOC){
			GPIOC_PCLK_DI();
		}else if(pGPIOx == GPIOD){
			GPIOD_PCLK_DI();
		}else if(pGPIOx == GPIOE){
			GPIOE_PCLK_DI();
		}else if(pGPIOx == GPIOF){
			GPIOF_PCLK_DI();
		}else if(pGPIOx == GPIOG){
			GPIOG_PCLK_DI();
		}else if(pGPIOx == GPIOH){
			GPIOH_PCLK_DI();
		}else if(pGPIOx == GPIOI){
			GPIOI_PCLK_DI();
		}
	}
}


/***************************************************************************************************************************
 * @Function - GPIO_ReadFromInputPort
 * @Brief    - It reads the data from the input port
 * @Param1   - GPIO base address
 * @Param2   -
 * @Param3   -
 * @Return   - 1 or 0 / SET or RESET macros / ENABLE or DISABLE macros(16 bit)
 * @Note     - None
 **********************************************************************************************************************************/
uint8_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t Value;
	Value = (uint16_t) pGPIOx ->IDR;
	return Value;

}

/***************************************************************************************************************************
 * @Function - GPIO_ReadFromInputPin
 * @Brief    - It reads the data from the input pin
 * @Param1   - GPIO base address
 * @Param2   - Pin number of the GPIO peripheral(8 bit)
 * @Param3   -
 * @Return   - 1 or 0 / SET or RESET macros / ENABLE or DISABLE macros(8 bit)
 * @Note     - None
 **********************************************************************************************************************************/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t) ((pGPIOx ->IDR >> PinNumber) & 0x00000001 );  //moving the bit corresponding to the pin we want to read to 0th bit position, Andig with 1 gives that bit value, while making all other bits zero.
	return value;

}

/***************************************************************************************************************************
 * @Function - GPIO_WriteToOutputPort
 * @Brief    - It give/send a data or value to the output port
 * @Param1   - GPIO base address
 * @Param2   - Value to send(16 bit)
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx ->ODR = Value;

}

/***************************************************************************************************************************
 * @Function - GPIO_WriteToOutputPin
 * @Brief    - It give/send a data or value to the output pin
 * @Param1   - GPIO base address
 * @Param2   - Pin number of the GPIO peripheral(8 bit)
 * @Param3   - Value to send(8 bit)
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET){
		//Write 1 to output data register
		pGPIOx ->ODR |= (1 << PinNumber);
	}else{
		//Write 0 to pin
		pGPIOx ->ODR &= ~(1 << PinNumber);
	}
}

/***************************************************************************************************************************
 * @Function - GPIO_ToggleOutputPin
 * @Brief    - It toggles the output pin of the GPIO peripheral
 * @Param1   - GPIO base address
 * @Param2   - Pin number of the GPIO peripheral(8 bit)
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx ->ODR ^= (1 << PinNumber);

}

/***************************************************************************************************************************
 * @Function - GPIO_IRQConfig
 * @Brief    - This gives the detail of the interrupts.
 * @Param1   - IRQ Number
 * @Param2   -
 * @Param3   - ENABLE or DISABLE macros
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)                                         // NVIC have several ISER and ICER registers each of 32 bits
	{                                                            // Each bit corresponds to a IRQNumber
		if(IRQNumber <= 31){                                     // so IRQNumber below 32 is handled by ISER0 and ICER0
			//Program ISER0 Register                             // IRQNumber from 32 to 63 is handled by ISER1 and ICER1
			*NVIC_ISER0 |= (1 << IRQNumber);

		}else if(IRQNumber >= 32 && IRQNumber <= 63){
			//Program ISER1 Register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));

		}else if(IRQNumber >= 64 && IRQNumber <= 96){
			//Program ISER1 Register
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}

	}else
	{
		if(IRQNumber <= 31){
			//Program ICER0 Register
			*NVIC_ICER0 |= (1 << IRQNumber);

		}else if(IRQNumber >= 32 && IRQNumber <= 63){
			//Program ICER1 Register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));

		}else if(IRQNumber >= 64 && IRQNumber <= 96){
			//Program ICER1 Register
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}

	}

}

/***************************************************************************************************************************
 * @Function - GPIO_IRQConfig Priority Configuration
 * @Brief    - This gives the detail of the Priority of the interrupts.
 * @Param1   - IRQ Number
 * @Param2   - IRQ Priority of the GPIO peripheral
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//Find IPR                                                                // NVIC have many IPR registers of 32 bits, each 8 bits are one section which give priority value to an IRQNumber
	uint8_t IPRx = IRQNumber / 4;                                             // IPRx = IRQNumber / 4 ; This gives the IPR register which is handling the particular IRQNumber
	//Find section in register                                                // IPRx_section = IRQNumber % 4 ; This gives the section of 8 bit which is handling that IRQNumber in that IPR register
	uint8_t IPRx_section = IRQNumber % 4;                                     // shift_amount = (8 * IPRx_section)+(8 - NO_PR_BITS_IMPLEMENTED) ; (8 * IPRx_section) gives the starting bit of that section and
	uint8_t shift_amount = (8 * IPRx_section) + (8 - NO_PR_BITS_IMPLEMENTED);   // NO_PR_BITS_IMPLEMENTED = 4, bcz there is only total 16 priority values (0 - 15) and it uses 4 upper/higher bits. 8 - 4 = 4
	*(NVIC_PR_BASEADDR + IPRx) &= ~(0xFF << (8 * IPRx_section));                // so in total it gives we need to shift that much and have to give the priority value there.
	*(NVIC_PR_BASEADDR + IPRx) |= (IRQPriority << shift_amount);              // clearing the bits and storing the value in the correct bit.

}

void GPIO_IRQHandling(uint8_t PinNumber)
{
	//Clear the EXTI PR register corresponding to the pin number we have used
	if(EXTI ->PR & (1 << PinNumber))             //if pending register corresponding to a pin number is set, then we need to clear it
	{                                                // by putting 1 to that PR.
		//clear the PR                           // PR is a special register with Write 1 to Clear(W1C) bahaviour.
		EXTI ->PR = (1 << PinNumber);            // While reading the register, 1 = pending interrupt and 0 = no pending interrupt
		                                         // But while writing, 0 = do nothing and 1 = Clear the pending flag. Its not giving the value 1 to flag, it just clear it
	}

}




























