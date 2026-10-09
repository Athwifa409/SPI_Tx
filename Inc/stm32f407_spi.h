/*
 * stm32f407_spi.h
 *
 *  Created on: 28-Sept-2026
 *      Author: athwi
 */

#ifndef STM32F407_SPI_H_
#define STM32F407_SPI_H_

#include "stm32f407xx.h"

/*
 * Configuration structure for spi peripherals
 */
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
	uint8_t SPI_LSBFIRST;

}SPI_Config_t;

/*
 * Handle structure for SPIx peripherals
 */
typedef struct
{
	SPI_RegDef_t   *pSPIx;           //Hold the base address of SPIx peripherals
	SPI_Config_t   SPIConfig;
	uint8_t        *pTxBuffer;       //To store the address of application data that needs to be transmitted.
	uint8_t        *pRxBuffer;       //To store the address of the place where the received data is storing.
	uint32_t       TxLen;            //To store TX length; This tell how much data remains to transmit
	uint32_t       RxLen;            //To store RX length; This tells how much data remains to receive
	uint8_t        TxState;          //To store TX state
	uint8_t        RxState;          //To store RX state

}SPI_Handle_t;



/******************************************************************************************************************************
 * Macros for SPI initialization APIs
 ***********************************************************************************************************************************/
/*
 * SPI Device mode - either master or slave
 */
#define SPI_DEVICE_MODE_MASTER       1    //device can have 2 mode - master or slave.
#define SPI_DEVICE_MODE_SLAVE        0    //It is controlled by SPI_CR1.MSTR = 0(slave) / 1(master)

/*
 * SPI bus configure - Simplex, Half Duplex, Full Duplex
 */
#define SPI_BUS_CONFIG_FD                    1   //FD is a 2 line unidirectional data mode(MOSI, MISO, SCK). BIDIMODE = 0 & RXONLY = 0
#define SPI_BUS_CONFIG_HD                    2   //HD is a single line bidirectional data mode(MOSI/MISO, SCK). BIDIMODE = 1, RXONLY = 0
                                                 //if BIDIMODE = 1, it can be either transmission(BIDIOE = 1) or reception(BIDIOE = 0)
#define SPI_BUS_CONFIG_SIMPLEX_RXOnly        3   //Board only receive data. BIDIMODE =0, RXONLY = 1

/*
 * SPI Serial Clock Speed - Determined by BR[2:0](Baud Rate Control) of CR1
 */
#define SPI_SCLK_SPEED_DIV2         0      //This is the baud rate of the SPI communication.
#define SPI_SCLK_SPEED_DIV4         1      //SCK is getting from the peripheral clock by dividing its frequency
#define SPI_SCLK_SPEED_DIV8         2      //BR[2:0] = 000 --> fPCL/2 ,BR[2:0] = 001 --> fPCL/4 , BR[2:0] = 010 --> fPCL/8 etc
#define SPI_SCLK_SPEED_DIV16        3
#define SPI_SCLK_SPEED_DIV32        4
#define SPI_SCLK_SPEED_DIV64        5
#define SPI_SCLK_SPEED_DIV128       6
#define SPI_SCLK_SPEED_DIV256       7

/*
 * SPI DFF(Data Frame Format) - EITHER 8 BITS OR 16 BITS
 */
#define SPI_DFF_8BITS         0      //This define the data frame format used by SPI communication.
#define SPI_DFF_16BITS        1      //for 8 bits, SPI_DR[7:0] and for 16 bits SPI_DR[15:0] is used.

/*
 * SPI_CPOL(Clock Polarity)
 */
#define SPI_CPOL_HIGH       1     //Clock polarity determine when the IDLE condition is happening in SCK
#define SPI_CPOL_LOW        0     //CPOL = 0 --> SCK IDLE LOW, CPOL = 1 --> SCK IDLE HIGH

/*
 * SPI_CPHA(Clock Phase)
 */
#define SPI_CPHA_HIGH      1    //This determines which clock transition is used for data capture
#define SPI_CPHA_LOW       0    //CPOL CPHA together decide the mode of the SPI. 00(mode 0), 01(mode 1) etc
                                //Master and slave must use same timing mode.

/*
 * SPI_SSM (software slave management) - Can be hardware or Software
 */
#define SPI_SSM_EN         1       //If SSM is enabled then internal NSS state is controlled by SSI. NSS is the signal used to select individual slaves.
#define SPI_SSM_DI         0       //NSS means Negative Slave Select/ Chip Select(when there is more than 1 slave)
                                   //If SSM is disabled, then we use Hardware NSS control.

/*
 * SPI related status flag definitions
 */
#define SPI_TXE_FLAG         (1 << SPI_SR_TXE)     //If TXE = 1, then 1 << 1 --> 00000010, then TXE flag becomes a mask.
                                                   //TXE means transmit buffer empty. When TXE=1, then we can load another data to it. It is cleared when writing to DR
#define SPI_RXNE_FLAG        (1 << SPI_SR_RXNE)    //RXNE means Receive buffer is not empty. When RXNE=1, received data is waiting.
                                                   //Then we Read DR, it clears RXNE
#define SPI_BUSY_FLAG        (1 << SPI_SR_BSY)     //BSY means Busy. If BSY=1, SPI communication is busy.
                                                   //This is used when we need to determine that communication has actually finished before disabling SPI.

/*
 * SPI application states
 */
#define SPI_READY           0        //These are not hardware values, but belong to the SPI_Handle_t driver.
#define SPI_BUSY_IN_RX      1        //TxState == READY --> BUSY_IN_TX --> READY
#define SPI_BUSY_IN_TX      2        //RxState == Ready --> BUSY_IN_RX --> READY
                                     //This is used bcz interrupt based communication is asynchronous and Tx and Rx states tells them whether it is transmitting or receiving at the moment

/*
 * Possible SPI Application events
 */
#define SPI_EVENT_TX_CMPLT        1   //They say the asynchronous transmission operation/ reception operation is completed.
#define SPI_EVENT_RX_CMPLT        2
#define SPI_EVENT_OVR_ERR         3   //Hardware support error interrupts using ERRIE.
#define SPI_EVENT_CRC_ERR         4   //ERRIE controls interrupt generation for things including CRCERR, OVR, MODF in SPI mode

/*
 * First send or receive bit is MSB/LSB
 */
#define SPI_FIRSTBIT_MSB    0
#define SPI_FIRSTBIT_LSB    1


/********************************************************************************************************************************
 *                          APIs supported by SPI
 *************************************************************************************************************************/
/*
 * SPI initialization and Deinitialization
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);  //
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 * Peripheral clock setup for SPI
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi); //have dedicated RCC peripheral clock enable registers.

/*
 * Data send and Receive for SPI
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);

/*
 * For IRQ Configuration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);  //This configures NVIC, which decide whether CPU receive interrupt or not.
                                                                 //if TXEIE = 1, then SPI is allowed to generate a TXE interrupt and NVIC must allow corresponding IRQ, otherwise ISR wont execute.
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority); //This configures the priority of interrupt in the NVIC.
                                                                     //NVIC decides which pending interrupt gets serviced first based on the priority.
void SPI_IRQHandling(SPI_Handle_t *pHandle);   //This examine TXE, RXNE, OVR, MODF, CRCERR and decide what to do with these registers.
                                               //This uses handle structure to handle IRQ

/*
 * For SPI enabling and Peripheral control
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);  //This control SPI_CR1.SPE, SPE = 0 --> SPI disabled, 1 --> SPI enabled

/*
 * SPI Configuration to avoid MODF error
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);  //When SSM = 1(software slave management) then SSI controls internal NSS state

/*
 * SSOE control peripheral to enable NSS in hardware NSS management
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);  //SS Output Enable, it controls hardware NSS output when configured appropriately.
                                                           //With SSOE = 1, SS output is enabled in master mode while peripheral is enabled.

/*
 * To get the flag status
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);  //It check whether a flag is set or not
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);   //For an interrupt based transmission, when TxState = BUSY_IN_TX, ISR keeps sending bytes till TxLen = 0.
                                                        //When TxLen = 0, then we disable TXE interrupt, clear Tx buffer and Tx Length. and TxState = READY
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);      //When RxLen = 0, it disable RXNE interrupt and clear Rx state and RxState = READY
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);          //OVR-Means overrun. it happens when new receive data arrives before previous data has been properly handled.
                                                   //The reference manual defines OVR as an overrun condition and specifies that it is cleared by a software sequence.
                                         //spi receives byte --> RXNE = 1 -->CPU should read DR, if another byte arrives, OVR = 1.

/*
 * Application call back
 */
void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle, uint8_t AppEv);  //The driver shouldn't necessarily decide what your application does after an event
                                                                    //Instead when event occures spi driver --> callback --> application




















#endif /* STM32F407_SPI_H_ */
