/**
 ******************************************************************************
 * @file    FLASH/FLASH_EraseProgram/Src/main.c
 * @author  MCD Application Team
 * @brief   This example provides a description of how to erase and program the
 *          STM32F7xx FLASH.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F7xx_HAL_Examples
 * @{
 */

/** @addtogroup FLASH_EraseProgram
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR ADDR_FLASH_SECTOR_2 /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR (ADDR_FLASH_SECTOR_6 - 1) /* End @ of user Flash area */

#define DATA_32 ((uint32_t)0x12345678)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t FirstSector = 0, NbOfSectors = 0;
uint32_t Address = 0, SECTORError = 0;
__IO uint32_t data32 = 0, MemoryProgramStatus = 0;
FLASH_OBProgramInitTypeDef OBInit;
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static uint32_t GetSector(uint32_t Address);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
   /* Enable the CPU Cache */
   CPU_CACHE_Enable();

   /* STM32F7xx HAL library initialization:
        - Configure the Flash prefetch
        - Systick timer is configured by default as source of time base, but user
          can eventually implement his proper time base source (a general purpose
          timer for example or other time source), keeping in mind that Time base
          duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
          handled in milliseconds basis.
        - Set NVIC Group Priority to 4
        - Low Level Initialization
      */
   HAL_Init();

   /* Configure the system clock to 216 MHz */
   SystemClock_Config();

   /* Initialize LED1, LED2 */
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED2);

   /* Unlock the Flash to enable the flash control register access *************/
   HAL_FLASH_Unlock();

   /* Allow Access to option bytes sector */
   HAL_FLASH_OB_Unlock();
   /* Get the Dual bank configuration status */
   HAL_FLASHEx_OBGetConfig(&OBInit);
#if defined(DUAL_BANK)
   /* Turn on LED3 if FLASH is configured in Dual Bank mode */
   if ((OBInit.USERConfig & OB_NDBANK_SINGLE_BANK) == OB_NDBANK_SINGLE_BANK)
#else
   /* Turn on LED3 if FLASH is configured in Single Bank mode */
   if ((OBInit.USERConfig & OB_NDBANK_SINGLE_BANK) == OB_NDBANK_DUAL_BANK)
#endif
   {
      while (1)
      {
         BSP_LED_On(LED1);
         BSP_LED_Off(LED2);
         ;
      }
   }

   /* Erase the user Flash area
     (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

   /* Get the 1st sector to erase */
   FirstSector = GetSector(FLASH_USER_START_ADDR);
   /* Get the number of sector to erase from 1st sector*/
   NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1;
   /* Fill EraseInit structure*/
   EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
   EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
   EraseInitStruct.Sector = FirstSector;
   EraseInitStruct.NbSectors = NbOfSectors;

   /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
      you have to make sure that these data are rewritten before they are accessed during code
      execution. If this cannot be done safely, it is recommended to flush the caches by setting the
      DCRST and ICRST bits in the FLASH_CR register. */
   if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
   {
      /*
        Error occurred while sector erase.
        User can add here some code to deal with this error.
        SECTORError will contain the faulty sector and then to know the code error on this sector,
        user can call function 'HAL_FLASH_GetError()'
      */
      /* Infinite loop */
      while (1)
      {
         /* Make LED1 blink (100ms on, 2s off) to indicate error in Erase operation */
         BSP_LED_On(LED1);
         HAL_Delay(100);
         BSP_LED_Off(LED1);
         HAL_Delay(2000);
      }
   }

   /* Program the user Flash area word by word
     (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

   Address = FLASH_USER_START_ADDR;

   while (Address < FLASH_USER_END_ADDR)
   {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
      {
         Address = Address + 4;
      }
      else
      {
         /* Error occurred while writing data in Flash memory.
            User can add here some code to deal with this error */
         while (1)
         {
            /* Make LED1 blink (100ms on, 2s off) to indicate error in Write operation */
            BSP_LED_On(LED1);
            HAL_Delay(100);
            BSP_LED_Off(LED1);
            HAL_Delay(2000);
         }
      }
   }

   /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
   HAL_FLASH_Lock();

   /* Check if the programmed data is OK
       MemoryProgramStatus = 0: data programmed correctly
       MemoryProgramStatus != 0: number of words not programmed correctly ******/
   Address = FLASH_USER_START_ADDR;
   MemoryProgramStatus = 0x0;

   while (Address < FLASH_USER_END_ADDR)
   {
      data32 = *(__IO uint32_t*)Address;

      if (data32 != DATA_32)
      {
         MemoryProgramStatus++;
      }
      Address = Address + 4;
   }

   /*Check if there is an issue to program data*/
   if (MemoryProgramStatus == 0)
   {
      /* No error detected. Switch on LED2*/
      BSP_LED_On(LED2);
   }
   else
   {
      /* Error detected. LED1 will blink with 1s period */
      while (1)
      {
         BSP_LED_On(LED1);
         HAL_Delay(1000);
         BSP_LED_Off(LED1);
         HAL_Delay(1000);
      }
   }

   /* Infinite loop */
   while (1)
   {
   }
}

/**
 * @brief  Gets the sector of a given address
 * @param  None
 * @retval The sector of a given address
 */
static uint32_t GetSector(uint32_t Address)
{
   uint32_t sector = 0;

   if ((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
   {
      sector = FLASH_SECTOR_0;
   }
   else if ((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
   {
      sector = FLASH_SECTOR_1;
   }
   else if ((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
   {
      sector = FLASH_SECTOR_2;
   }
   else if ((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
   {
      sector = FLASH_SECTOR_3;
   }
   else if ((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
   {
      sector = FLASH_SECTOR_4;
   }
   else if ((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
   {
      sector = FLASH_SECTOR_5;
   }
   else if ((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
   {
      sector = FLASH_SECTOR_6;
   }
   else if ((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
   {
      sector = FLASH_SECTOR_7;
   }
   else if ((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
   {
      sector = FLASH_SECTOR_8;
   }
   else if ((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
   {
      sector = FLASH_SECTOR_9;
   }
   else if ((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
   {
      sector = FLASH_SECTOR_10;
   }
#if defined(DUAL_BANK)
   else if ((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
   {
      sector = FLASH_SECTOR_11;
   }
   else if ((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
   {
      sector = FLASH_SECTOR_12;
   }
   else if ((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
   {
      sector = FLASH_SECTOR_13;
   }
   else if ((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
   {
      sector = FLASH_SECTOR_14;
   }
   else if ((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
   {
      sector = FLASH_SECTOR_15;
   }
   else if ((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
   {
      sector = FLASH_SECTOR_16;
   }
   else if ((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
   {
      sector = FLASH_SECTOR_17;
   }
   else if ((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
   {
      sector = FLASH_SECTOR_18;
   }
   else if ((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
   {
      sector = FLASH_SECTOR_19;
   }
   else if ((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
   {
      sector = FLASH_SECTOR_20;
   }
   else if ((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
   {
      sector = FLASH_SECTOR_21;
   }
   else if ((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
   {
      sector = FLASH_SECTOR_22;
   }
   else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23) */
   {
      sector = FLASH_SECTOR_23;
   }
#else
   else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
   {
      sector = FLASH_SECTOR_11;
   }
#endif /* DUAL_BANK */
   return sector;
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 216000000
 *            HCLK(Hz)                       = 216000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 432
 *            PLL_P                          = 2
 *            PLL_Q                          = 9
 *            PLL_R                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 7
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void)
{
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_OscInitTypeDef RCC_OscInitStruct;
   HAL_StatusTypeDef ret = HAL_OK;

   /* Enable Power Control clock */
   __HAL_RCC_PWR_CLK_ENABLE();

   /* The voltage scaling allows optimizing the power consumption when the device is
      clocked below the maximum system frequency, to update the voltage scaling value
      regarding system frequency refer to product datasheet.  */
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 25;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;
   RCC_OscInitStruct.PLL.PLLR = 7;

   ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
   if (ret != HAL_OK)
   {
      while (1)
      {
         ;
      }
   }

   /* Activate the OverDrive to reach the 216 MHz Frequency */
   ret = HAL_PWREx_EnableOverDrive();
   if (ret != HAL_OK)
   {
      while (1)
      {
         ;
      }
   }

   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
   RCC_ClkInitStruct.ClockType =
      (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

   ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
   if (ret != HAL_OK)
   {
      while (1)
      {
         ;
      }
   }
}

/**
 * @brief  CPU L1-Cache enable.
 * @param  None
 * @retval None
 */
static void CPU_CACHE_Enable(void)
{
   /* Enable I-Cache */
   SCB_EnableICache();

   /* Enable D-Cache */
   SCB_EnableDCache();
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
   /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

   /* Infinite loop */
   while (1)
   {
   }
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
