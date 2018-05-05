/**
 ******************************************************************************
 * @file    Examples_LL/RTC/RTC_Calendar/Src/main.c
 * @author  MCD Application Team
 * @brief   This sample code shows how to use STM32F7xx RTC LL API to configure
 *          Time and Date.
 *          Peripheral initialization done using LL unitary services functions.
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

/** @addtogroup STM32F7xx_LL_Examples
 * @{
 */

/** @addtogroup RTC_Calendar
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Oscillator time-out values */
#define LSI_TIMEOUT_VALUE ((uint32_t)2) /* 2 ms */
#define LSE_TIMEOUT_VALUE ((uint32_t)5000) /* 5 s */
#define RTC_TIMEOUT_VALUE ((uint32_t)1000) /* 1 s */

/* Defines related to Clock configuration */
/* Uncomment to enable the adequate Clock Source */
#define RTC_CLOCK_SOURCE_LSE
//#define RTC_CLOCK_SOURCE_LSI

#ifdef RTC_CLOCK_SOURCE_LSI
/* ck_apre=LSIFreq/(ASYNC prediv + 1) with LSIFreq=32 kHz RC */
#   define RTC_ASYNCH_PREDIV ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#   define RTC_SYNCH_PREDIV ((uint32_t)0x00F9)
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
/* ck_apre=LSEFreq/(ASYNC prediv + 1) = 256Hz with LSEFreq=32768Hz */
#   define RTC_ASYNCH_PREDIV ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#   define RTC_SYNCH_PREDIV ((uint32_t)0x00FF)
#endif

/* Define used to indicate date/time updated */
#define RTC_BKP_DATE_TIME_UPDTATED ((uint32_t)0x32F2)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Buffers used for displaying Time and Date */
uint8_t aShowTime[50] = { 0 };
uint8_t aShowDate[50] = { 0 };

#if (USE_TIMEOUT == 1)
uint32_t Timeout = 0; /* Variable used for Timeout management */
#endif /* USE_TIMEOUT */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Configure_RTC_Clock(void);
void Configure_RTC(void);
void Configure_RTC_Calendar(void);
uint32_t Enter_RTC_InitMode(void);
uint32_t Exit_RTC_InitMode(void);
uint32_t WaitForSynchro_RTC(void);
void Show_RTC_Calendar(void);
void LED_Init(void);
void LED_On(void);
void LED_Blinking(uint32_t Period);
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

   /* Configure the system clock to 216 MHz */
   SystemClock_Config();

   /* Initialize LED1 */
   LED_Init();

   /*##-Configure the RTC peripheral #######################################*/
   Configure_RTC_Clock();

   /*##-Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
   /* Read the Back Up Register 1 Data */
   if (LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR1) != RTC_BKP_DATE_TIME_UPDTATED)
   {
      /*##-Configure the RTC peripheral #######################################*/
      Configure_RTC();

      /* Configure RTC Calendar */
      Configure_RTC_Calendar();
   }

   /* Turn-on LED1 to indicate that calendar has been well configured */
   LED_On();

   /* Infinite loop */
   while (1)
   {
      /*##-3- Display the updated Time and Date ################################*/
      Show_RTC_Calendar();
   }
}

/**
 * @brief  Configure RTC clock.
 * @param  None
 * @retval None
 */
void Configure_RTC_Clock(void)
{
   /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
   /* To change the source clock of the RTC feature (LSE, LSI), you have to:
      - Enable the power clock
      - Enable write access to configure the RTC clock source (to be done once after reset).
      - Reset the Back up Domain
      - Configure the needed RTC clock source */
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
   LL_PWR_EnableBkUpAccess();

   /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
#ifdef RTC_CLOCK_SOURCE_LSE
   /* Enable LSE only if disabled.*/
   if (LL_RCC_LSE_IsReady() == 0)
   {
      LL_RCC_ForceBackupDomainReset();
      LL_RCC_ReleaseBackupDomainReset();
      LL_RCC_LSE_Enable();
#   if (USE_TIMEOUT == 1)
      Timeout = LSE_TIMEOUT_VALUE;
#   endif /* USE_TIMEOUT */
      while (LL_RCC_LSE_IsReady() != 1)
      {
#   if (USE_TIMEOUT == 1)
         if (LL_SYSTICK_IsActiveCounterFlag())
         {
            Timeout--;
         }
         if (Timeout == 0)
         {
            /* LSE activation error */
            LED_Blinking(LED_BLINK_ERROR);
         }
#   endif /* USE_TIMEOUT */
      }
      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
   }
#elif defined(RTC_CLOCK_SOURCE_LSI)
   /* Enable LSI */
   LL_RCC_LSI_Enable();
#   if (USE_TIMEOUT == 1)
   Timeout = LSI_TIMEOUT_VALUE;
#   endif /* USE_TIMEOUT */
   while (LL_RCC_LSI_IsReady() != 1)
   {
#   if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
         Timeout--;
      }
      if (Timeout == 0)
      {
         /* LSI activation error */
         LED_Blinking(LED_BLINK_ERROR);
      }
#   endif /* USE_TIMEOUT */
   }
   /* Reset backup domain only if LSI is not yet selected as RTC clock source */
   if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSI)
   {
      LL_RCC_ForceBackupDomainReset();
      LL_RCC_ReleaseBackupDomainReset();
      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
   }

#else
#   error "configure clock for RTC"
#endif
}

/**
 * @brief  Configure RTC.
 * @note   Peripheral configuration is minimal configuration from reset values.
 *         Thus, some useless LL unitary functions calls below are provided as
 *         commented examples - setting is default configuration from reset.
 * @param  None
 * @retval None
 */
void Configure_RTC(void)
{
   /*##-1- Enable RTC peripheral Clocks #######################################*/
   /* Enable RTC Clock */
   LL_RCC_EnableRTC();

   /*##-2- Disable RTC registers write protection ##############################*/
   LL_RTC_DisableWriteProtection(RTC);

   /*##-3- Enter in initialization mode #######################################*/
   if (Enter_RTC_InitMode() != RTC_ERROR_NONE)
   {
      /* Initialization Error */
      LED_Blinking(LED_BLINK_ERROR);
   }

   /*##-4- Configure RTC ######################################################*/
   /* Configure RTC prescaler and RTC data registers */
   /* Set Hour Format */
   LL_RTC_SetHourFormat(RTC, LL_RTC_HOURFORMAT_AMPM);
   /* Set Asynch Prediv (value according to source clock) */
   LL_RTC_SetAsynchPrescaler(RTC, RTC_ASYNCH_PREDIV);
   /* Set Synch Prediv (value according to source clock) */
   LL_RTC_SetSynchPrescaler(RTC, RTC_SYNCH_PREDIV);
   /* Set OutPut */
   /* Reset value is LL_RTC_ALARMOUT_DISABLE */
   // LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_DISABLE);
   /* Set OutPutPolarity */
   /* Reset value is LL_RTC_OUTPUTPOLARITY_PIN_HIGH */
   // LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_HIGH);
   /* Set OutPutType */
   /* Reset value is LL_RTC_ALARM_OUTPUTTYPE_OPENDRAIN */
   // LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_OPENDRAIN);

   /*##-5- Exit of initialization mode #######################################*/
   Exit_RTC_InitMode();

   /*##-6- Enable RTC registers write protection #############################*/
   LL_RTC_EnableWriteProtection(RTC);
}

/**
 * @brief  Configure the current time and date.
 * @param  None
 * @retval None
 */
void Configure_RTC_Calendar(void)
{
   /*##-1- Disable RTC registers write protection ############################*/
   LL_RTC_DisableWriteProtection(RTC);

   /*##-2- Enter in initialization mode ######################################*/
   if (Enter_RTC_InitMode() != RTC_ERROR_NONE)
   {
      /* Initialization Error */
      LED_Blinking(LED_BLINK_ERROR);
   }

   /*##-3- Configure the Date ################################################*/
   /* Note: __LL_RTC_CONVERT_BIN2BCD helper macro can be used if user wants to*/
   /*       provide directly the decimal value:                               */
   /*       LL_RTC_DATE_Config(RTC, LL_RTC_WEEKDAY_MONDAY,                    */
   /*                          __LL_RTC_CONVERT_BIN2BCD(31), (...))           */
   /* Set Date: Friday December 29th 2016 */
   LL_RTC_DATE_Config(RTC, LL_RTC_WEEKDAY_FRIDAY, 0x29, LL_RTC_MONTH_DECEMBER, 0x16);

   /*##-4- Configure the Time ################################################*/
   /* Set Time: 11:59:55 PM*/
   LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_PM, 0x11, 0x59, 0x55);

   /*##-5- Exit of initialization mode #######################################*/
   if (Exit_RTC_InitMode() != RTC_ERROR_NONE)
   {
      /* Initialization Error */
      LED_Blinking(LED_BLINK_ERROR);
   }

   /*##-6- Enable RTC registers write protection #############################*/
   LL_RTC_EnableWriteProtection(RTC);

   /*##-8- Writes a data in a RTC Backup data Register1 #######################*/
   LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR1, RTC_BKP_DATE_TIME_UPDTATED);
}

/**
 * @brief  Enter in initialization mode
 * @note In this mode, the calendar counter is stopped and its value can be updated
 * @param  None
 * @retval RTC_ERROR_NONE if no error
 */
uint32_t Enter_RTC_InitMode(void)
{
   /* Set Initialization mode */
   LL_RTC_EnableInitMode(RTC);

#if (USE_TIMEOUT == 1)
   Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

   /* Check if the Initialization mode is set */
   while (LL_RTC_IsActiveFlag_INIT(RTC) != 1)
   {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
         Timeout--;
      }
      if (Timeout == 0)
      {
         return RTC_ERROR_TIMEOUT;
      }
#endif /* USE_TIMEOUT */
   }

   return RTC_ERROR_NONE;
}

/**
 * @brief  Exit Initialization mode
 * @param  None
 * @retval RTC_ERROR_NONE if no error
 */
uint32_t Exit_RTC_InitMode(void)
{
   LL_RTC_DisableInitMode(RTC);

   /* Wait for synchro */
   /* Note: Needed only if Shadow registers is enabled           */
   /*       LL_RTC_IsShadowRegBypassEnabled function can be used */
   return (WaitForSynchro_RTC());
}

/**
 * @brief  Wait until the RTC Time and Date registers (RTC_TR and RTC_DR) are
 *         synchronized with RTC APB clock.
 * @param  None
 * @retval RTC_ERROR_NONE if no error (RTC_ERROR_TIMEOUT will occur if RTC is
 *         not synchronized)
 */
uint32_t WaitForSynchro_RTC(void)
{
   /* Clear RSF flag */
   LL_RTC_ClearFlag_RS(RTC);

#if (USE_TIMEOUT == 1)
   Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

   /* Wait the registers to be synchronised */
   while (LL_RTC_IsActiveFlag_RS(RTC) != 1)
   {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
         Timeout--;
      }
      if (Timeout == 0)
      {
         return RTC_ERROR_TIMEOUT;
      }
#endif /* USE_TIMEOUT */
   }
   return RTC_ERROR_NONE;
}

/**
 * @brief  Display the current time and date.
 * @param  None
 * @retval None
 */
void Show_RTC_Calendar(void)
{
   /* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */
   /* Display time Format : hh:mm:ss */
   sprintf((char*)aShowTime,
           "%.2d:%.2d:%.2d",
           __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC)),
           __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC)),
           __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC)));
   /* Display date Format : mm-dd-yy */
   sprintf((char*)aShowDate,
           "%.2d-%.2d-%.2d",
           __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC)),
           __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC)),
           2000 + __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC)));
}

/**
 * @brief  Initialize LED1.
 * @param  None
 * @retval None
 */
void LED_Init(void)
{
   /* Enable the LED1 Clock */
   LED1_GPIO_CLK_ENABLE();

   /* Configure IO in output push-pull mode to drive external LED1 */
   LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
   /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
   // LL_GPIO_SetPinOutputType(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
   /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
   // LL_GPIO_SetPinSpeed(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
   /* Reset value is LL_GPIO_PULL_NO */
   // LL_GPIO_SetPinPull(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_PULL_NO);
}

/**
 * @brief  Turn-on LED1.
 * @param  None
 * @retval None
 */
void LED_On(void)
{
   /* Turn LED1 on */
   LL_GPIO_SetOutputPin(LED1_GPIO_PORT, LED1_PIN);
}

/**
 * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
 * @param  Period : Period of time (in ms) between each toggling of LED
 *   This parameter can be user defined values. Pre-defined values used in that example are :
 *     @arg LED_BLINK_FAST : Fast Blinking
 *     @arg LED_BLINK_SLOW : Slow Blinking
 *     @arg LED_BLINK_ERROR : Error specific Blinking
 * @retval None
 */
void LED_Blinking(uint32_t Period)
{
   /* Toggle IO in an infinite loop */
   while (1)
   {
      LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
      LL_mDelay(Period);
   }
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
 *            HSI Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 432
 *            PLL_P                          = 2
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 7
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
   /* Enable HSE clock */
   LL_RCC_HSE_EnableBypass();
   LL_RCC_HSE_Enable();
   while (LL_RCC_HSE_IsReady() != 1)
   {
   };

   /* Set FLASH latency */
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);

   /* Enable PWR clock */
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

   /* Activation OverDrive Mode */
   LL_PWR_EnableOverDriveMode();
   while (LL_PWR_IsActiveFlag_OD() != 1)
   {
   };

   /* Activation OverDrive Switching */
   LL_PWR_EnableOverDriveSwitching();
   while (LL_PWR_IsActiveFlag_ODSW() != 1)
   {
   };

   /* Main PLL configuration and activation */
   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 432, LL_RCC_PLLP_DIV_2);
   LL_RCC_PLL_Enable();
   while (LL_RCC_PLL_IsReady() != 1)
   {
   };

   /* Sysclk activation on the main PLL */
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
   while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   };

   /* Set APB1 & APB2 prescaler */
   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
   LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

   /* Set systick to 1ms */
   SysTick_Config(216000000 / 1000);

   /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
   SystemCoreClock = 216000000;
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

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
