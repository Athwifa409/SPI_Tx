/*
 * stm32f407xx.h
 *
 *  Created on: 28-Sept-2026
 *      Author: athwi
 */

#ifndef STM32F407XX_H_
#define STM32F407XX_H_

#include <stddef.h>
#include <stdint.h>

#define __vo volatile
#define __weak __attribute__((weak))

/*
 * Base addresses of flash and SRAM(from datasheet)
 */
#define FLASH_BASEADDR    0x08000000U
#define SRAM1_BASEADDR    0x20000000U
#define SRAM2_BASEADDR    0x2001C000U
#define ROM_BASEADDR      0x1FFF0000U
#define SRAM_BASEADDR     SRAM1_BASEADDR

/*
 * Bus peripheral base addresses
 */
#define PERIPH_BASEADDR    0x40000000U
#define APB1_BASEADDR      PERIPH_BASEADDR
#define APB2_BASEADDR      0x40010000U
#define AHB1_BASEADDR      0x40020000U
#define AHB2_BASEADDR      0x50000000U

/*
 * Base addresses of peripherals hanging on APB1 bus
 */
#define SPI2_BASEADDR    (APB1_BASEADDR + 0x3800)
#define SPI3_BASEADDR    (APB1_BASEADDR + 0x3C00)
#define USART2_BASEADDR    (APB1_BASEADDR + 0x4400)
#define USART3_BASEADDR    (APB1_BASEADDR + 0x4800)
#define UART4_BASEADDR    (APB1_BASEADDR + 0x4C00)
#define UART5_BASEADDR    (APB1_BASEADDR + 0x5000)
#define I2C1_BASEADDR    (APB1_BASEADDR + 0x5400)
#define I2C2_BASEADDR    (APB1_BASEADDR + 0x5800)
#define I2C3_BASEADDR    (APB1_BASEADDR + 0x5C00)

/*
 * Base addresses of peripherals hanging on APB2 bus
 */
#define USART1_BASEADDR    (APB2_BASEADDR + 0x1000)
#define USART6_BASEADDR    (APB2_BASEADDR + 0x1400)
#define SPI1_BASEADDR    (APB2_BASEADDR + 0x3000)
#define SYSCFG_BASEADDR    (APB2_BASEADDR + 0x3800)
#define EXTI_BASEADDR    (APB2_BASEADDR + 0x3C00)

/*
 * Base addresses of peripherals hanging on AHB1 bus
 */
#define GPIOA_BASEADDR    (AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR    (AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR    (AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR    (AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR    (AHB1_BASEADDR + 0x1000)
#define GPIOF_BASEADDR    (AHB1_BASEADDR + 0x1400)
#define GPIOG_BASEADDR    (AHB1_BASEADDR + 0x1800)
#define GPIOH_BASEADDR    (AHB1_BASEADDR + 0x1C00)
#define GPIOI_BASEADDR    (AHB1_BASEADDR + 0x2000)

#define RCC_BASEADDR      (AHB1_BASEADDR + 0x3800)



/****************************************************Processor Specific Details************************************************************
 *                                ARM Cortex M4 Processor NVIC_ISERx and NVIC_ICERx register addresses
 ********************************************************************************************************************************************/
/*
 * ARM Cortex M4 NVIC_ISER (Interrupt Set Enable Register) register addresses
 */
#define NVIC_ISER0     ((__vo uint32_t *)0xE000E100)
#define NVIC_ISER1     ((__vo uint32_t *)0xE000E104)
#define NVIC_ISER2     ((__vo uint32_t *)0xE000E108)
#define NVIC_ISER3     ((__vo uint32_t *)0xE000E10C)
#define NVIC_ISER4     ((__vo uint32_t *)0xE000E110)
#define NVIC_ISER5     ((__vo uint32_t *)0xE000E114)
#define NVIC_ISER6     ((__vo uint32_t *)0xE000E118)
#define NVIC_ISER7     ((__vo uint32_t *)0xE000E11C)

/*
 * ARM Cortex M4 NVIC_ICER (Interrupt Clear Enable Register) register addresses
 */
#define NVIC_ICER0     ((__vo uint32_t *)0xE000E180)
#define NVIC_ICER1     ((__vo uint32_t *)0xE000E184)
#define NVIC_ICER2     ((__vo uint32_t *)0xE000E188)
#define NVIC_ICER3     ((__vo uint32_t *)0xE000E18C)
#define NVIC_ICER4     ((__vo uint32_t *)0xE000E190)
#define NVIC_ICER5     ((__vo uint32_t *)0xE000E194)
#define NVIC_ICER6     ((__vo uint32_t *)0xE000E198)
#define NVIC_ICER7     ((__vo uint32_t *)0xE000E19C)

/*
 * ARM Cortex M4 NVIC_IPR (Interrupt Priority Registers) register addresses
 */
#define NVIC_IPR0      ((__vo uint32_t *)0xE000E400)
#define NVIC_IPR1      ((__vo uint32_t *)0xE000E404)
#define NVIC_IPR2      ((__vo uint32_t *)0xE000E408)
#define NVIC_IPR3      ((__vo uint32_t *)0xE000E40C)
#define NVIC_IPR4      ((__vo uint32_t *)0xE000E410)
#define NVIC_IPR5      ((__vo uint32_t *)0xE000E414)

/*
 * ARM Cortex Mx Processor Priority Register Address Calculation
 */
#define NVIC_PR_BASEADDR    ((__vo uint32_t*)0xE000E400)

/*
 * ARM Cortex Mx Processor number of priority bits implemented in Priority Register
 */
#define NO_PR_BITS_IMPLEMENTED     4



/***************************************Peripheral register definition structures*****************************************************/
/*
 * Peripheral register definition structure for GPIOs
 */
typedef struct
{
	__vo uint32_t MODER;       //GPIO port mode register. Address offset: 0x00
	__vo uint32_t OTYPER;      //GPIO port output type register. Address offset: 0x04
	__vo uint32_t OSPEEDR;     //GPIO port output speed register. Address offset: 0x08
	__vo uint32_t PUPDR;       //GPIO port pull-up/pull-down register. Address offset: 0x0C
	__vo uint32_t IDR;         //GPIO port input data register. Address offset: 0x10
	__vo uint32_t ODR;         //GPIO port output data register. Address offset: 0x14
	__vo uint32_t BSRR;        //GPIO port bit set/reset register. Address offset: 0x18
	__vo uint32_t LCKR;        //GPIO port configuration lock register. Address offset: 0x1C
	__vo uint32_t AFR[2];        //GPIO alternate function low register. Address offset: 0x20
                               //GPIO alternate function high register. Address offset: 0x24

}GPIO_RegDef_t;

/*
 * Peripheral register definition structure for RCC
 */
typedef struct
{
	__vo uint32_t CR;              //RCC clock control register. Address offset: 0x00
	__vo uint32_t PLLCFGR;         //RCC PLL configuration register. Address offset: 0x04
	__vo uint32_t CFGR;            //RCC clock configuration register. Address offset: 0x08
	__vo uint32_t CIR;             //RCC clock interrupt register. Address offset: 0x0C
	__vo uint32_t AHBRSTR[2];      //RCC AHB1 peripheral reset register. Address offset: 0x10
	                               //RCC AHB2 peripheral reset register. Address offset: 0x14
	 uint32_t     RESERVED0;
	__vo uint32_t APBRSTR[2];      //RCC APB1 peripheral reset register. Address offset: 0x20
	                               //RCC APB2 peripheral reset register. Address offset: 0x24
	 uint32_t     RESERVED1[2];
	__vo uint32_t AHBENR[2];       //RCC AHB1 peripheral clock enable register. Address offset: 0x30
	                               //RCC AHB2 peripheral clock enable register. Address offset: 0x34
	 uint32_t     RESERVED2;
	__vo uint32_t APBENR[2];       //RCC APB1 peripheral clock enable register. Address offset: 0x40
	                               //RCC APB2 peripheral clock enable register. Address offset: 0x44
	 uint32_t     RESERVED3[2];
	__vo uint32_t AHBLPENR[2];     //RCC AHB1 peripheral clock enable in low power mode register. Address offset: 0x50
	                               //RCC AHB2 peripheral clock enable in low power mode register. Address offset: 0x54
	 uint32_t     RESERVED4;
	__vo uint32_t APBLPENR[2];     //RCC APB1 peripheral clock enable in low power mode register. Address offset: 0x60
	                               //RCC APB2 peripheral clock enabled in low power mode register. Address offset: 0x64
	 uint32_t     RESERVED5[2];
	__vo uint32_t BDCR;            //RCC Backup domain control register. Address offset: 0x70
	__vo uint32_t CSR;             //RCC clock control & status register. Address offset: 0x74
	 uint32_t     RESERVED6[2];
	__vo uint32_t SSCGR;           //RCC spread spectrum clock generation register. Address offset: 0x80
	__vo uint32_t PLLI2SCFGR;      //RCC PLLI2S configuration register. Address offset: 0x84

}RCC_RegDef_t;

/*
 * Peripheral register definition structure for EXTI
 */
typedef struct
{
	__vo uint32_t IMR;     //Interrupt mask register. Address offset: 0x00
	__vo uint32_t EMR;     //Event mask register. Address offset: 0x04
	__vo uint32_t RTSR;    //Rising trigger selection register. Address offset: 0x08
	__vo uint32_t FTSR;    //Falling trigger selection register. Address offset: 0x0C
	__vo uint32_t SWIER;   //Software interrupt event register. Address offset: 0x10
	__vo uint32_t PR;      //Pending register. Address offset: 0x14

}EXTI_RegDef_t;

/*
 * Peripheral register definition structure for SYSCFG
 */
typedef struct
{
	__vo uint32_t MEMRMP;      //SYSCFG memory remap register. Address offset: 0x00
	__vo uint32_t PMC;         //SYSCFG peripheral mode configuration register. Address offset: 0x04
	__vo uint32_t EXTICR[4];   //SYSCFG external interrupt configuration register 1. Address offset: 0x08
	                           //SYSCFG external interrupt configuration register 2. Address offset: 0x0C
	                           //SYSCFG external interrupt configuration register 3. Address offset: 0x10
	                           //SYSCFG external interrupt configuration register 4. Address offset: 0x14
	__vo uint32_t CMPCR;       //Compensation cell control register. Address offset: 0x20

}SYSCFG_RegDef_t;

/*
 * Peripheral register definition structure for SPI
 */
typedef struct
{
	__vo uint16_t CR1;        //SPI control register 1. Address offset: 0x00
	__vo uint16_t CR2;        //SPI control register 2. Address offset: 0x04
	__vo uint16_t SR;         //SPI status register. Address offset: 0x08
	__vo uint16_t DR;         //SPI data register. Address offset: 0x0C
	__vo uint16_t CRCPR;      //SPI CRC polynomial register. Address offset: 0x10
	__vo uint16_t RXCRCR;     //SPI RX CRC register. Address offset: 0x14
	__vo uint16_t TXCRCR;     //SPI TX CRC register. Address offset: 0x18
	__vo uint16_t I2SCFGR;    //SPI_I2S configuration register. Address offset: 0x1C
	__vo uint16_t I2SPR;      //SPI_I2S prescaler register. Address offset: 0x20

}SPI_RegDef_t;



/**********************************************MACROS**************************************/
/*
 * Initializing the register structures for future easy use
 */
#define GPIOA        ((GPIO_RegDef_t *)GPIOA_BASEADDR)
#define GPIOB        ((GPIO_RegDef_t *)GPIOB_BASEADDR)
#define GPIOC        ((GPIO_RegDef_t *)GPIOC_BASEADDR)
#define GPIOD        ((GPIO_RegDef_t *)GPIOD_BASEADDR)
#define GPIOE        ((GPIO_RegDef_t *)GPIOE_BASEADDR)
#define GPIOF        ((GPIO_RegDef_t *)GPIOF_BASEADDR)
#define GPIOG        ((GPIO_RegDef_t *)GPIOG_BASEADDR)
#define GPIOH        ((GPIO_RegDef_t *)GPIOH_BASEADDR)
#define GPIOI        ((GPIO_RegDef_t *)GPIOI_BASEADDR)

#define RCC          ((RCC_RegDef_t *)RCC_BASEADDR)

#define EXTI         ((EXTI_RegDef_t *)EXTI_BASEADDR)

#define SYSCFG       ((SYSCFG_RegDef_t *)SYSCFG_BASEADDR)

#define SPI1         ((SPI_RegDef_t *)SPI1_BASEADDR)
#define SPI2         ((SPI_RegDef_t *)SPI2_BASEADDR)
#define SPI3         ((SPI_RegDef_t *)SPI3_BASEADDR)

/*
 * Clock enable macros for GPIOx peripherals
 */
#define GPIOA_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 0))
#define GPIOB_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 1))
#define GPIOC_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 2))
#define GPIOD_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 3))
#define GPIOE_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 4))
#define GPIOF_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 5))
#define GPIOG_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 6))
#define GPIOH_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 7))
#define GPIOI_PCLK_EN()      (RCC ->AHBENR[0] |= (1 << 8))

/*
 * Clock enable macros for SPIx peripherals
 */
#define SPI1_PCLK_EN()      (RCC ->APBENR[1] |= (1 << 12))
#define SPI2_PCLK_EN()      (RCC ->APBENR[0] |= (1 << 14))
#define SPI3_PCLK_EN()      (RCC ->APBENR[0] |= (1 << 15))

/*
 * Clock enable macros for SYSCFG peripherals
 */
#define SYSCFG_PCLK_EN()     (RCC ->APBENR[1] |= (1 << 14))

/*
 * Clock disable macros for GPIOx peripherals
 */
#define GPIOA_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 0))
#define GPIOB_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 1))
#define GPIOC_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 2))
#define GPIOD_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 3))
#define GPIOE_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 4))
#define GPIOF_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 5))
#define GPIOG_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 6))
#define GPIOH_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 7))
#define GPIOI_PCLK_DI()      (RCC ->AHBENR[0] &= ~(1 << 8))

/*
 * Clock disable macros for SPIx peripherals
 */
#define SPI1_PCLK_DI()       (RCC ->APBENR[1] &= ~(1 << 12))
#define SPI2_PCLK_DI()       (RCC ->APBENR[0] &= ~(1 << 14))
#define SPI3_PCLK_DI()       (RCC ->APBENR[0] &= ~(1 << 15))

/*
 * Clock disable macros for SYSCFG peripherals
 */
#define SYSCFG_PCLK_DI()     (RCC ->APBENR[1] &= ~(1 << 14))

/*
 * Macros to reset GPIOx peripherals
 */
#define GPIOA_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 0)); (RCC ->AHBRSTR[0] &= ~(1 << 0)); } while(0)
#define GPIOB_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 1)); (RCC ->AHBRSTR[0] &= ~(1 << 1)); } while(0)
#define GPIOC_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 2)); (RCC ->AHBRSTR[0] &= ~(1 << 2)); } while(0)
#define GPIOD_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 3)); (RCC ->AHBRSTR[0] &= ~(1 << 3)); } while(0)
#define GPIOE_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 4)); (RCC ->AHBRSTR[0] &= ~(1 << 4)); } while(0)
#define GPIOF_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 5)); (RCC ->AHBRSTR[0] &= ~(1 << 5)); } while(0)
#define GPIOG_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 6)); (RCC ->AHBRSTR[0] &= ~(1 << 6)); } while(0)
#define GPIOH_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 7)); (RCC ->AHBRSTR[0] &= ~(1 << 7)); } while(0)
#define GPIOI_REG_RESET()      do{ (RCC ->AHBRSTR[0] |= (1 << 8)); (RCC ->AHBRSTR[0] &= ~(1 << 8)); } while(0)

/*
 * Macros to reset SPIx peripherals
 */
#define SPI1_REG_RESET()       do{ (RCC ->APBRSTR[1] |= (1 << 12)); (RCC ->APBRSTR[1] &= ~(1 << 12)); } while(0)
#define SPI2_REG_RESET()       do{ (RCC ->APBRSTR[0] |= (1 << 14)); (RCC ->APBRSTR[1] &= ~(1 << 14)); } while(0)
#define SPI3_REG_RESET()       do{ (RCC ->APBRSTR[1] |= (1 << 15)); (RCC ->APBRSTR[1] &= ~(1 << 15)); } while(0)

/*
 * Return port code for a given GPIOx base address
 */
#define GPIO_BASEADDR_TO_CODE(x)    ((x == GPIOA) ? 0 :\
		                             (x == GPIOB) ? 1 :\
									 (x == GPIOC) ? 2 :\
									 (x == GPIOD) ? 3 :\
									 (x == GPIOE) ? 4 :\
									 (x == GPIOF) ? 5 :\
									 (x == GPIOG) ? 6 :\
									 (x == GPIOH) ? 7 :\
									 (x == GPIOI) ? 8 : 0 )

/*
 * Some generic macros
 */
#define ENABLE           1
#define DISABLE          0
#define SET              ENABLE
#define RESET            DISABLE
#define GPIO_PIN_SET     SET
#define GPIO_PIN_RESET   RESET
#define FLAG_RESET       RESET
#define FLAG_SET         SET

/*
 * IRQ(Interrupt Request) number of stm32f407xx MCU
 */
#define IRQ_NO_EXTI0      6
#define IRQ_NO_EXTI1      7
#define IRQ_NO_EXTI2      8
#define IRQ_NO_EXTI3      9
#define IRQ_NO_EXTI4      10
#define IRQ_NO_EXTI9_5    23
#define IRQ_NO_EXTI15_10  40

/*
 * Macros for all the possible priority levels
 */
#define NVIC_IRQ_PRI0      0
#define NVIC_IRQ_PRI1      1
#define NVIC_IRQ_PRI2      2
#define NVIC_IRQ_PRI3      3
#define NVIC_IRQ_PRI4      4
#define NVIC_IRQ_PRI5      5
#define NVIC_IRQ_PRI6      6
#define NVIC_IRQ_PRI7      7
#define NVIC_IRQ_PRI8      8
#define NVIC_IRQ_PRI9      9
#define NVIC_IRQ_PRI10     10
#define NVIC_IRQ_PRI11     11
#define NVIC_IRQ_PRI12     12
#define NVIC_IRQ_PRI13     13
#define NVIC_IRQ_PRI14     14
#define NVIC_IRQ_PRI15     15



/******************************************************************************************
 *Bit position definitions of SPI peripheral
 ******************************************************************************************/
/*
 * Bit position definitions SPI_CR1
 */
#define SPI_CR1_CPHA     				 0
#define SPI_CR1_CPOL      				 1
#define SPI_CR1_MSTR     				 2
#define SPI_CR1_BR   					 3
#define SPI_CR1_SPE     				 6
#define SPI_CR1_LSBFIRST   			 	 7
#define SPI_CR1_SSI     				 8
#define SPI_CR1_SSM      				 9
#define SPI_CR1_RXONLY      		 	10
#define SPI_CR1_DFF     			 	11
#define SPI_CR1_CRCNEXT   			 	12
#define SPI_CR1_CRCEN   			 	13
#define SPI_CR1_BIDIOE     			 	14
#define SPI_CR1_BIDIMODE      			15

/*
 * Bit position definitions SPI_CR2
 */
#define SPI_CR2_RXDMAEN		 			0
#define SPI_CR2_TXDMAEN				 	1
#define SPI_CR2_SSOE				 	2
#define SPI_CR2_FRF						4
#define SPI_CR2_ERRIE					5
#define SPI_CR2_RXNEIE				 	6
#define SPI_CR2_TXEIE					7

/*
 * Bit position definitions SPI_SR
 */
#define SPI_SR_RXNE						0
#define SPI_SR_TXE				 		1
#define SPI_SR_CHSIDE				 	2
#define SPI_SR_UDR					 	3
#define SPI_SR_CRCERR				 	4
#define SPI_SR_MODF					 	5
#define SPI_SR_OVR					 	6
#define SPI_SR_BSY					 	7
#define SPI_SR_FRE					 	8

#include "stm32f407_gpio.h"
#include "stm32f407_spi.h"



#endif /* STM32F407XX_H_ */
