/*
 * stm32f407_spi.c
 *
 *  Created on: 28-Sept-2026
 *      Author: athwi
 */


#include "stm32f407xx.h"
#include "stm32f407_spi.h"
#include "stm32f407_gpio.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

/*
 * The APIs and their meaning are noted below :
 */

/***************************************************************************************************************************
 * @Function - SPI_Init
 * @Brief    - This function initializes the SPI peripheral in use
 * @Param1   - SPI Handler function/address
 * @Param2   -
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle)       //Initialize the SPI peripheral according to the configuration stored in the handle
{
	//Configuring the SPI_CR1 register
	uint32_t tempreg = 0;

	//Enable the peripheral clock
	SPI_PeriClockControl(pSPIHandle ->pSPIx, ENABLE);  //enable the peripheral clock

	//1.Configure the device mode
	tempreg |= pSPIHandle ->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	//2.Configuring bus configuration
	if(pSPIHandle ->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//BIDI MODE should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);

	}else if(pSPIHandle ->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//BIDI MODE should be set
		tempreg |= (1 << SPI_CR1_BIDIMODE);

	}else if(pSPIHandle ->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXOnly)
	{
		//BIDI MODE should be cleared and RXOnly should be set
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		tempreg |= (1 << SPI_CR1_RXONLY);

	}
	//3.Configure the SPI Serial clock speed(baud rate)
	tempreg |= pSPIHandle ->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4.Configure the DFF
	tempreg |= pSPIHandle ->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	//5.Configure the CPOL
	tempreg |= pSPIHandle ->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	//6.Configure the CPHA
	tempreg |= pSPIHandle ->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	//7.Configure the SSM
	tempreg |= pSPIHandle ->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	//8.Configure the LSBFIRST
	tempreg |= pSPIHandle ->SPIConfig.SPI_LSBFIRST << SPI_CR1_LSBFIRST;

}

/***************************************************************************************************************************
 * @Function - SPI_DeInit
 * @Brief    - This function de-initializes the SPI pin. De-initialization means returns it to its reset state.
 * @Param1   - SPI base address
 * @Param2   -
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void SPI_DeInit(SPI_RegDef_t *pSPIx)   //Reset the peripherals into initial values.
{
	if(pSPIx == SPI1){
		SPI1_REG_RESET();
	}else if(pSPIx == SPI2){
		SPI2_REG_RESET();
	}
	else if(pSPIx == SPI3){
		SPI3_REG_RESET();
	}

}

/***************************************************************************************************************************
 * @Function - SPI_PeriClockControl
 * @Brief    - They control the peripheral clock of the SPI pins
 * @Param1   - SPI base address
 * @Param2   - ENABLE or DISABLE macros
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE){               //enabling the peripheral clock, else disable
		if(pSPIx == SPI1){              //the board have dedicated RCC peripheral clock enable registers
			SPI1_PCLK_EN();             // RCC --> SPI clock enabled --> SPI registers operating
		}else if(pSPIx == SPI2){        //Driver can determine appropriate RCC enable bit from the peripheral address
			SPI2_PCLK_EN();
		}else if(pSPIx == SPI3){
			SPI3_PCLK_EN();
		}

	}else{
		if(pSPIx == SPI1){
			SPI1_PCLK_DI();
		}else if(pSPIx == SPI2){
			SPI2_PCLK_DI();
		}else if(pSPIx == SPI3){
			SPI3_PCLK_DI();
		}

	}
}

/***************************************************************************************************************************
 * @Function - SPI_SendData
 * @Brief    - This function transmit the data from SPI peripheral to a slave/ outside
 * @Param1   - SPI pin
 * @Param2   -TxBuffer
 * @Param3   -Length of the data
 * Data from CPU --> SPI_DR --> if Tx buffer is 0 then TX buffer, else wait --> Shift register send 1 bit --> Len --
 * --> if Len != 0, Shift again , send next bit --> Through MOSI send it to slave.
 * if TXE  = 0, data send completed and next data comes to TX Buffer and repeat
 * SCK provide timing for the serial communication
 **********************************************************************************************************************************/
/*
 * Function for SPI_SendData
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx ->SR & FlagName)    //FOR TXE, SR & TXE --> TXE Flag set --> Transmit
	{
		return FLAG_SET;
	}
	return FLAG_RESET;

}

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)       //This is blocking or polling transmit fn
{
	while(Len > 0)                                                           //while(Len > 0) {
	{                                                                        //    while(TXE == 0)  --> Write data to DR
		//1.Wait until Tx buffer is empty                                    //              }
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		//2.CHeck DFF bit in CR1                                            //
		if((pSPIx ->CR1 & (1 << SPI_CR1_DFF)))
		{
			//16 bit DFF, Load 16 bit data to DR
			pSPIx ->DR = *((uint16_t *)pTxBuffer);

			//Decrement Len 2 times
			Len--;
			Len--;

			//Increment TxBuffer 2 times
			(uint16_t *)pTxBuffer++;
			(uint16_t *)pTxBuffer++;

		}else
		{
			//8 bit DFF bit in CR1
			pSPIx ->DR = *pTxBuffer;

			//Decrement Len once
			Len--;

			//Increment TxBuffer
			(uint8_t *)pTxBuffer++;

		}

	}

}

/***************************************************************************************************************************
 * @Function - SPI_PeripheralControl
 * @Brief    - To make spi enable
 * @Param1   - SPI pin
 * @Param2   - enable / disable
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx ->CR1 |= (1 << SPI_CR1_SPE);

	}else
	{
		pSPIx ->CR1 &= ~(1 << SPI_CR1_SPE);
	}

}

/*********************************************************************
 * @fn      		  - SPI_SSIConfig
 * @brief             -
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 * @return            -
 * @Note              -
 *******************************************************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx ->CR1 |= (1 << SPI_CR1_SSI);

	}else
	{
		pSPIx ->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}
/***************************************************************************************************************************
 * @Function - SPI2_SSOEConfig
 * @Brief    - To enable NSS in hardware NSS management
 * @Param1   - SPI pin
 * @Param2   - enable / disable
 * @Param3   -
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void SPI2_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx ->CR2 |= (1 << SPI_CR2_SSOE);

	}else
	{
		pSPIx ->CR2 &= ~(1 << SPI_CR2_SSOE);

	}

}

/***************************************************************************************************************************
 * @Function - SPI_ReceiveData
 * @Brief    - This function receive the data from SPI peripheral to a slave/ outside
 * @Param1   - SPI pin
 * @Param2   - RxBuffer
 * @Param3   - Length of the data
 * Through MISO --> Shift register get the data bit by bit --> Goes to RX Buffer --> If RXNE is 0, read DR register then repeat the process
 **********************************************************************************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len > 0)                                                           // while(Len > 0){
	{                                                                        //       while(RXNE == 0);
		//1.Wait until Rx buffer is empty                                    //            Read DR; }
		while(SPI_GetFlagStatus (pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);       // Reading DR clears RXNE

		//2.Check DFF bit in CR1
		if((pSPIx ->CR1 & (1 << SPI_CR1_DFF)))
		{
			//16 BIT DFF, Load 16 bit data to DR
			*((uint16_t *)pRxBuffer) = pSPIx ->DR;

			//Decrement Len 2 times
			Len--;
			Len--;

			//Increment TxBuffer
			(uint16_t *)pRxBuffer++;
			(uint16_t *)pRxBuffer++;

		}else
		{
			//8 BIT DFF, Load 8 bit data to DR
			*((uint8_t *)pRxBuffer) = pSPIx ->DR;

			//Decrement Len once
			Len--;

			//Increment TxBuffer
			(uint16_t *)pRxBuffer++;

		}

	}

}

/***************************************************************************************************************************
 * @Function - GPIO_IRQConfig
 * @Brief    - This gives the detail of the interrupts.
 * @Param1   - IRQ Number
 * @Param2   - IRQ Priority of the GPIO peripheral
 * @Param3   - ENABLE or DISABLE macros
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//Find the IPR
	uint8_t IPRx = IRQNumber / 4;
	//Find section in the register
	uint8_t IPRx_section = IRQNumber % 4;
	//bits to be shifted to get the desired bit
	uint8_t shift_amount = (8 * IPRx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + IPRx) |= (IRQPriority << shift_amount);

}

/***************************************************************************************************************************
 * @Function - SPI_SendData Interrupt
 * @Brief    - This function transmit the data from SPI peripheral to a slave/ outside
 * @Param1   - SPI pin
 * @Param2   -TxBuffer
 * @Param3   -Length of the data
 * @Note     - Using this, instead of checking and waiting the cpu can do other works till spi event occurs and can do ISR.
 * SPI interrupt sources include TXE, RXNE, MODF, OVR, CRCERR with control bits like TXEIE, RXNEIE, ERRIE in SPI_CR2
 **********************************************************************************************************************************/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTXBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle ->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		//1.Save the Tx buffer address and len information in some global variables
		pSPIHandle ->pTxBuffer = pTXBuffer;
		pSPIHandle ->TxLen = Len;

		//2.Mark the SPI state as busy in transmission so that no other
		// code can take over SPI peripheral until transmission is over
		pSPIHandle ->TxState = SPI_BUSY_IN_TX;

		//3.Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle -> pSPIx ->CR2 |= (1 << SPI_CR2_TXEIE);

		//4.Data transmission will be handled by the ISR Code
	}
	return state;

}

/***************************************************************************************************************************
 * @Function - SPI_ReceiveData Interrupt
 * @Brief    - This function receive the data from SPI peripheral to a slave/ outside
 * @Param1   - SPI pin
 * @Param2   - RxBuffer
 * @Param3   - Length of the data
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle ->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		//1.Save the Tx buffer address and len information in some global variables
		pSPIHandle ->pRxBuffer = pRxBuffer;
		pSPIHandle ->RxLen = Len;

		//2.Mark the SPI state as busy in transmission so that no other
		// code can take over SPI peripheral until transmission is over
		pSPIHandle ->RxState = SPI_BUSY_IN_RX;

		//3.Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle -> pSPIx ->CR2 |= (1 << SPI_CR2_RXNEIE);

		//4.Data transmission will be handled by the ISR Code
	}
	return state;

}

/***************************************************************************************************************************
 * @Function - GPIO_IRQConfig
 * @Brief    - This gives the detail of the interrupts.
 * @Param1   - IRQ Number
 * @Param2   - IRQ Priority of the GPIO peripheral
 * @Param3   - ENABLE or DISABLE macros
 * @Return   - None
 * @Note     - None
 **********************************************************************************************************************************/
__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	//This is a weak implementation, the application may override this function.
}

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)   //if TXE = 1 and TXEIE is enabled then SPI interrupt occurs.
{                                                                // ISR then knows SPI transmit buffer need another data item.
	//2.Check DFF bit in CR1
	if((pSPIHandle ->pSPIx ->CR1 & (1 << SPI_CR1_DFF)))
	{
		//16 bit DFF, Load 16 bit data to DR
		pSPIHandle ->pSPIx ->DR = *((uint16_t *)pSPIHandle ->pTxBuffer);

		//Decrement Len 2 times
		pSPIHandle ->TxLen--;
		pSPIHandle ->TxLen--;

		//Increment TxBuffer
		(uint16_t *)pSPIHandle ->pTxBuffer++;

	}else
	{
		//8 bit DFF, Load 8 bit data in CR1
		pSPIHandle ->pSPIx ->DR = *((uint8_t *)pSPIHandle ->pTxBuffer);

		//Decrement Len 2 times
		pSPIHandle ->TxLen--;
		pSPIHandle ->TxLen--;

		//Increment TxBuffer
		(uint8_t *)pSPIHandle ->pTxBuffer++;
	}

	if(!pSPIHandle ->TxLen)
	{
		//TxLen is zero, so close the spi communication and inform the application that Tx is over.
		//This prevents interrupts from setting up of TXE  flag
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);

	}

}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)           //if RXNE = 1 and RXNEIE is enabled then it causes a receive interrupt.
{                                                                         //then ISR read SPI_DR and store in RxBuffer, inc pointer and dec RxLen
	//2.Check DFF bit in CR1
	if((pSPIHandle ->pSPIx ->CR1 & (1 << SPI_CR1_DFF)))
	{
		//16 bit DFF, Load 16 bit data to DR
		pSPIHandle ->pSPIx ->DR = *((uint16_t *)pSPIHandle ->pRxBuffer);

		//Decrement Len 2 times
		pSPIHandle ->RxLen--;
		pSPIHandle ->RxLen--;

		//Increment TxBuffer
		(uint16_t *)pSPIHandle ->pRxBuffer++;
		(uint16_t *)pSPIHandle ->pRxBuffer++;

	}else
	{
		//8 bit DFF, Load 8 bit data in CR1
		pSPIHandle ->pSPIx ->DR = *((uint8_t *)pSPIHandle ->pRxBuffer);

		//Decrement Len 2 times
		pSPIHandle ->RxLen--;

		//Increment TxBuffer
		(uint8_t *)pSPIHandle ->pRxBuffer++;
	}

	if(!pSPIHandle ->RxLen)
	{
		//TxLen is zero, so close the spi communication and inform the application that Tx is over.
		//This prevents interrupts from setting up of RXNE flag
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);

	}

}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;
	//1.Clear the over error flag
	if(pSPIHandle ->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle ->pSPIx ->DR;
		temp = pSPIHandle ->pSPIx ->SR;

	}
	(void)temp;

	//2.Inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);

}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle ->pSPIx ->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle ->pTxBuffer = NULL;
	pSPIHandle ->TxLen = 0;
	pSPIHandle ->TxState = SPI_READY;

}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle ->pSPIx ->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle ->pRxBuffer = NULL;
	pSPIHandle ->RxLen = 0;
	pSPIHandle ->RxState = SPI_READY;

}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx ->DR;
	temp = pSPIx ->SR;
	(void)temp;

}

void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp1, temp2;

	//Check for TXE
	temp1 = pSPIHandle ->pSPIx ->SR & (1 << SPI_SR_TXE);
	temp2 = pSPIHandle ->pSPIx ->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
	{
		//Handle TXE
		spi_txe_interrupt_handle(pSPIHandle);

	}

	//Check for RXNE
	temp1 = pSPIHandle ->pSPIx ->SR & (1 << SPI_SR_RXNE);
	temp2 = pSPIHandle ->pSPIx ->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		//Handle TXE
		spi_rxne_interrupt_handle(pSPIHandle);

	}

	//Check for OVR Flag
	temp1 = pSPIHandle ->pSPIx ->SR & (1 << SPI_SR_OVR);
	temp2 = pSPIHandle ->pSPIx ->CR2 & (1 << SPI_SR_OVR);

	if(temp1 && temp2)
	{
		//Handle TXE
		spi_ovr_err_interrupt_handle(pSPIHandle);

	}

}


