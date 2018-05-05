/**
 ******************************************************************************
 * @file    LCD_DSI/LCD_DSI_ULPM_Data/Src/main.c
 * @author  MCD Application Team
 * @brief   This example describes how to operate the DSI ULPM (Ultra Low Power Mode)
 *          on data lane only in a use case with display in WVGA Landscape
 *          of size (800x480) using the STM32F7xx HAL API and BSP.
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
#include "candies_800x480_argb8888.h"
#include <string.h>
#include <stdio.h>

/** @addtogroup STM32F7xx_HAL_Examples
 * @{
 */

/** @addtogroup LCD_DSI_ULPM_Data
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
extern DSI_HandleTypeDef hdsi_eval;
extern LTDC_HandleTypeDef hltdc_eval;
static DMA2D_HandleTypeDef hdma2d;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t frameCnt = 0;
char str_display[60] = "";

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void OnError_Handler(uint32_t condition);
static uint8_t CheckForUserInput(void);
static void CopyPicture(uint32_t* pSrc, uint32_t* pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  On Error Handler on condition TRUE.
 * @param  condition : Can be TRUE or FALSE
 * @retval None
 */
static void OnError_Handler(uint32_t condition)
{
   if (condition)
   {
      BSP_LED_On(LED3);
      while (1)
      {
         ;
      } /* Blocking on error */
   }
}

/**
 * @brief  Toggle Leds.
 * @param  None
 * @retval None
 */
void Toggle_Leds(void)
{
   static uint32_t ticks = 0;

   if (ticks++ > 1000)
   {
      BSP_LED_Toggle(LED1);
      ticks = 0;
   }
}

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
   /* Enable the CPU Cache */
   CPU_CACHE_Enable();

   uint8_t lcd_status = LCD_OK;
   uint8_t sdram_status = SDRAM_OK;

   /* This sample code displays a fixed image 800x480 on LCD KoD in */
   /* orientation mode landscape and DSI mode video burst           */

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

   /* Configure the system clock to 200 MHz */
   SystemClock_Config();

   /* Initialize used Leds */
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED3);

   /* Configure Tamper push-button */
   BSP_PB_Init(BUTTON_TAMPER, BUTTON_MODE_GPIO);

   /* Initialize the SDRAM */
   sdram_status = BSP_SDRAM_Init();
   OnError_Handler(sdram_status != SDRAM_OK);

   /* Initialize the LCD DSI in Video Burst mode with LANDSCAPE orientation */
   lcd_status = BSP_LCD_Init();
   OnError_Handler(lcd_status != LCD_OK);

   /* Program a line event at line 0 */
   HAL_LTDC_ProgramLineEvent(&hltdc_eval, 0);

   /* Copy texture to be displayed on LCD from Flash to SDRAM */
   CopyPicture((uint32_t*)&candies_800x480_argb8888,
               (uint32_t*)LCD_FB_START_ADDRESS,
               0,
               0,
               BSP_LCD_GetXSize(),
               BSP_LCD_GetYSize());

   BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER_BACKGROUND, LCD_FB_START_ADDRESS);
   BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);

   /* Prepare area to display frame number in the image displayed on LCD */
   BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
   BSP_LCD_FillRect(0, 400, BSP_LCD_GetXSize(), 80);
   BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
   BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
   BSP_LCD_SetFont(&Font16);

   /* Display title */
   BSP_LCD_DisplayStringAt(0, 420, (uint8_t*)"LCD_DSI_ULPM_Data example", CENTER_MODE);
   BSP_LCD_DisplayStringAt(0, 440, (uint8_t*)"Press TAMPER button to enter ULPM", CENTER_MODE);

   BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
   BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
   BSP_LCD_SetFont(&Font16);

   /* Infinite loop */
   while (1)
   {
      /* Clear previous line */
      BSP_LCD_ClearStringLine(460);

      /* New text to display */
      sprintf(str_display, ">> Frame Nb : %lu", frameCnt);

      /* Print updated frame number */
      BSP_LCD_DisplayStringAt(0, 460, (uint8_t*)str_display, CENTER_MODE);

      if (CheckForUserInput() > 0)
      {
         /* Clear previous line */
         BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
         BSP_LCD_ClearStringLine(440);
         BSP_LCD_DisplayStringAt(0, 440, (uint8_t*)"Enter ULPM - switch Off LCD 6 seconds", CENTER_MODE);
         BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

         /* Display Off with ULPM management Data lane only integrated */
         BSP_LCD_DisplayOff();
         HAL_Delay(1000);

         /* Switch Off bit LTDCEN */
         __HAL_LTDC_DISABLE(&hltdc_eval);

         /* Enter ultra low power mode (data lane only integrated) */
         HAL_DSI_EnterULPMData(&hdsi_eval);
         BSP_LED_On(LED1);

         HAL_Delay(6000);

         BSP_LCD_ClearStringLine(440);
         BSP_LCD_DisplayStringAt(
            0, 440, (uint8_t*)" Exited ULPM with success - Press To enter Again ULPM. ", CENTER_MODE);

         /* Exit ultra low power mode (data lane only integrated) */
         HAL_DSI_ExitULPMData(&hdsi_eval);
         BSP_LED_Off(LED1);

         /* Switch On bit LTDCEN */
         __HAL_LTDC_ENABLE(&hltdc_eval);

         /* Display On with ULPM exit Data lane only integrated */
         BSP_LCD_DisplayOn();
      }
   }
}

/**
 * @brief  Check for user input.
 * @param  None
 * @retval Input state (1 : active / 0 : Inactive)
 */
static uint8_t CheckForUserInput(void)
{
   if (BSP_PB_GetState(BUTTON_TAMPER) == GPIO_PIN_RESET)
   {
      HAL_Delay(10);
      do
      {
         /* Clear previous line */
         BSP_LCD_ClearStringLine(460);

         /* New text to display */
         sprintf(str_display, ">> Frame Nb : %lu", frameCnt);

         /* Print updated frame number */
         BSP_LCD_DisplayStringAt(0, 460, (uint8_t*)str_display, CENTER_MODE);

      } while (BSP_PB_GetState(BUTTON_TAMPER) == GPIO_PIN_RESET);

      return 1;
   }
   return 0;
}

/**
 * @brief  Line Event callback.
 * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
 *                the configuration information for the LTDC.
 * @retval None
 */
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef* hltdc)
{
   /* Increment frame count */
   frameCnt++;

   HAL_LTDC_ProgramLineEvent(hltdc, 0);
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 200000000
 *            HCLK(Hz)                       = 200000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 400
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

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 25;
   RCC_OscInitStruct.PLL.PLLN = 400;
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
 * @brief  Converts a line to an ARGB8888 pixel format.
 * @param  pSrc: Pointer to source buffer
 * @param  pDst: Output color
 * @param  xSize: Buffer width
 * @param  ColorMode: Input color mode
 * @retval None
 */
static void CopyPicture(uint32_t* pSrc, uint32_t* pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{
   uint32_t destination = (uint32_t)pDst + (y * 800 + x) * 4;
   uint32_t source = (uint32_t)pSrc;

   /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
   hdma2d.Init.Mode = DMA2D_M2M;
   hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
   hdma2d.Init.OutputOffset = 800 - xsize;
   hdma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
   hdma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR; /* No Output Red & Blue swap */

   /*##-2- DMA2D Callbacks Configuration ######################################*/
   hdma2d.XferCpltCallback = NULL;

   /*##-3- Foreground Configuration ###########################################*/
   hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
   hdma2d.LayerCfg[1].InputAlpha = 0xFF;
   hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
   hdma2d.LayerCfg[1].InputOffset = 0;
   hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
   hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

   hdma2d.Instance = DMA2D;

   /* DMA2D Initialization */
   if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)
   {
      if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
      {
         if (HAL_DMA2D_Start(&hdma2d, source, destination, xsize, ysize) == HAL_OK)
         {
            /* Polling For DMA transfer */
            HAL_DMA2D_PollForTransfer(&hdma2d, 100);
         }
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
